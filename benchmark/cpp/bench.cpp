// Benchmark of the C++ implementation:
//
//   cl /nologo /std:c++17 /O2 /EHsc /utf-8 /I ..\..\c++\include bench.cpp /Fe:bench.exe && bench.exe
//
// Prints one line per operation, "<name>\t<median milliseconds>", after warming up and taking the
// median of several rounds. The index sequence, the element count and the timing discipline are
// the same in all three implementations, so the numbers can be read side by side.

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <sv/SV_vector.hpp>

namespace {

constexpr std::size_t kSize = 100'000;
constexpr int kRounds = 21;

/// Keeps the optimiser from deleting the work whose time is being measured.
double sink = 0.0;

/// The pseudo-random index sequence, identical in all three implementations.
std::vector<sv::index_type> make_indexes(std::size_t count) {
  std::vector<sv::index_type> out;
  out.reserve(count);
  std::uint32_t state = 12345;
  for (std::size_t i = 0; i < count; ++i) {
    state = (state * 1103515245u + 12345u) & 0x7FFFFFFFu;
    out.push_back(static_cast<sv::index_type>(state % 2000000u) - 1000000);
  }
  return out;
}

const std::vector<sv::index_type> kIndexes = make_indexes(kSize);

double median(std::vector<double> samples) {
  std::sort(samples.begin(), samples.end());
  return samples[samples.size() / 2];
}

template <class Prepare, class Run>
void time_it(const std::string& name, Prepare prepare, Run run) {
  for (int round = 0; round < 3; ++round) {
    auto warmup = prepare();
    run(warmup);
  }
  std::vector<double> samples;
  samples.reserve(kRounds);
  for (int round = 0; round < kRounds; ++round) {
    auto input = prepare();
    const auto start = std::chrono::steady_clock::now();
    run(input);
    const auto end = std::chrono::steady_clock::now();
    samples.push_back(std::chrono::duration<double, std::milli>(end - start).count());
  }
  std::cout << name << '\t' << median(std::move(samples)) << '\n';
}

sv::SV_vector<double> filled() {
  sv::SV_vector<double> vector(0.0);
  for (std::size_t i = 0; i < kSize; ++i) {
    vector.set(kIndexes[i], static_cast<double>(i + 1));
  }
  return vector;
}

sv::SV_vector<double> sevens() {
  sv::SV_vector<double> vector(0.0);
  for (std::size_t i = 0; i < kSize; ++i) {
    vector.set(kIndexes[i], 7.0);
  }
  return vector;
}

std::vector<sv::SV_element<double>> elements() {
  std::vector<sv::SV_element<double>> out;
  out.reserve(kSize);
  for (std::size_t i = 0; i < kSize; ++i) {
    out.push_back(sv::SV_element<double>{kIndexes[i], static_cast<double>(i + 1)});
  }
  return out;
}

const std::vector<sv::SV_element<double>> kElements = elements();

using Pair = std::pair<sv::SV_vector<double>, sv::SV_vector<double>>;

}  // namespace

int main() {
  {
    const sv::SV_vector<double> probe = filled();
    std::cout << "stored\t" << probe.get_element_amount() << '\n';
  }

  time_it("set", [] { return 0; }, [](const int&) {
    sv::SV_vector<double> vector(0.0);
    for (std::size_t i = 0; i < kSize; ++i) {
      vector.set(kIndexes[i], static_cast<double>(i + 1));
    }
    sink += static_cast<double>(vector.get_element_amount());
  });

  time_it(
      "get", filled,
      [](const sv::SV_vector<double>& vector) {
        double sum = 0.0;
        for (std::size_t i = 0; i < kSize; ++i) {
          sum += vector.get(kIndexes[i]);
        }
        sink += sum;
      });

  time_it("from_elements", [] { return 0; }, [](const int&) {
    const sv::SV_vector<double> vector = sv::SV_vector<double>::from_elements(kElements, 0.0);
    sink += static_cast<double>(vector.get_element_amount());
  });

  time_it(
      "iterate", filled,
      [](const sv::SV_vector<double>& vector) {
        double sum = 0.0;
        for (const auto& element : vector) {
          sum += element.second;
        }
        sink += sum;
      });

  time_it(
      "replace default (full prune)", sevens,
      [](sv::SV_vector<double>& vector) { sink += vector.set_default_value(7.0) ? 1.0 : 0.0; });

  time_it(
      "replace predicate (full prune)", filled,
      [](sv::SV_vector<double>& vector) {
        sink += vector.set_equality([](const double&, const double&) { return true; }) ? 1.0 : 0.0;
      });

  // The pair is built once and reused: rebuilding it every round would leave the heap churned
  // right before the timed region, and both operations are pure reads.
  const Pair equal_pair(filled(), filled());
  Pair differing_pair(filled(), filled());
  differing_pair.second.set(kIndexes[kSize - 1], 999999999.0);

  time_it(
      "is_equal_to (equal)", [&] { return &equal_pair; },
      [](const Pair* pair) { sink += pair->first.is_equal_to(pair->second) ? 1.0 : 0.0; });

  time_it(
      "differences (one element)", [&] { return &differing_pair; },
      [](const Pair* pair) {
        sink += static_cast<double>(pair->first.differences(pair->second).size());
      });

  std::cout << "sink\t" << sink << '\n';
  return 0;
}
