// The one main() in the suite; every test file registers itself with the
// registry before this runs.

#include "sv_test.hpp"

int main() { return ::sv_test::run_all(); }
