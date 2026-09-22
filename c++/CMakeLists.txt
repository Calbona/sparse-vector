cmake_minimum_required(VERSION 3.21)

project(sparse-vector
  VERSION 1.1.0
  DESCRIPTION "A sparse vector: a mapping from integer index to arbitrary value that stores only the positions differing from a default value."
  HOMEPAGE_URL "https://github.com/Calbona/sparse-vector"
  LANGUAGES CXX)

# Before the target, not after: CMAKE_INSTALL_INCLUDEDIR has to be defined by the
# time the INSTALL_INTERFACE generator expression below is written, or it expands
# to nothing and consumers compile without an include path.
include(GNUInstallDirs)

# Header-only: an INTERFACE target with no compiled sources of its own.
add_library(sparse-vector INTERFACE)
add_library(Calbona::sparse-vector ALIAS sparse-vector)

# INTERFACE libraries have no CXX_STANDARD property, so the requirement is
# expressed as a usage requirement instead: whoever links this gets C++17.
target_compile_features(sparse-vector INTERFACE cxx_std_17)
target_include_directories(sparse-vector INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Consumed via add_subdirectory() or FetchContent, there is no reason to build
# this project's tests. Consumed as the top-level project, there is no reason not
# to. PROJECT_IS_TOP_LEVEL is 3.21 and up.
option(SPARSE_VECTOR_BUILD_TESTS "Build the sparse-vector test suite" ${PROJECT_IS_TOP_LEVEL})

if(SPARSE_VECTOR_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

# --- install -----------------------------------------------------------------

include(CMakePackageConfigHelpers)

# Only include/ is installed, so test/sv_test.hpp never reaches a consumer.
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS sparse-vector EXPORT sparse-vector-targets)

install(EXPORT sparse-vector-targets
  FILE sparse-vector-targets.cmake
  NAMESPACE Calbona::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sparse-vector)

configure_package_config_file(
  cmake/sparse-vector-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/sparse-vector-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sparse-vector)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/sparse-vector-config-version.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/sparse-vector-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/sparse-vector-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/sparse-vector)

install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/sparse-vector)
