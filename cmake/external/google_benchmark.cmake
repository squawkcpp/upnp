# - download and build google benchmark.
# Once done this will define
#  BENCHMARK_INCLUDE_DIR - The benchmark include directory.
#  BENCHMARK_LIBRARIES - The libraries needed to use benchmark

include(ExternalProject)

ExternalProject_Add(
  google_benchmark
  URL "https://github.com/google/benchmark/archive/master.zip"
  INSTALL_COMMAND ""
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  BUILD_BYPRODUCTS "google_benchmark-prefix/src/google_benchmark-build/src/libbenchmark.a"
)
ExternalProject_Get_Property(google_benchmark source_dir)
set(BENCHMARK_INCLUDE_DIR ${source_dir}/include/)
ExternalProject_Get_Property(google_benchmark binary_dir)
set(BENCHMARK_LIBRARIES ${binary_dir}/src/${CMAKE_FIND_LIBRARY_PREFIXES}benchmark.a)
set(BENCHMARK_LIBRARY benchmark)
add_library(${BENCHMARK_LIBRARY} STATIC IMPORTED)
set_property(TARGET ${BENCHMARK_LIBRARY} PROPERTY IMPORTED_LOCATION ${BENCHMARK_LIBRARIES} )
add_dependencies(${BENCHMARK_LIBRARY} google_benchmark)
