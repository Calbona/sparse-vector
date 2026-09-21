# One executable for the whole suite: the harness keeps a single registry shared
# by every translation unit, and test_main.cpp supplies the only main().
add_executable(sv_tests
  test_main.cpp
  test_default_value.cpp
  test_reading.cpp
  test_writing.cpp
  test_pruning.cpp
  test_introspection.cpp
  test_from_elements.cpp)

target_link_libraries(sv_tests PRIVATE Calbona::sparse-vector)

if(MSVC)
  # /WX is deliberately absent. MSVC's C4127 ("conditional expression is
  # constant") fires on checks such as SV_CHECK_EQ(vector.size(), 0u), and
  # fighting the compiler over its own macro expansion is not worth it.
  # /utf-8 because this checkout's path contains non-ASCII characters.
  target_compile_options(sv_tests PRIVATE /W4 /permissive- /utf-8 /EHsc)
else()
  target_compile_options(sv_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME sv.unit COMMAND sv_tests)
