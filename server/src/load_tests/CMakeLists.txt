cmake_minimum_required(VERSION 3.1)

set(TARGET "tester")
add_executable(tester tester.cpp)

target_include_directories(${TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/load_tests")

target_compile_features(${TARGET} PUBLIC cxx_std_14)

set_target_properties(${TARGET} PROPERTIES
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON)

target_compile_options(${TARGET}
  PRIVATE -Wall
  PRIVATE -Wextra
  PRIVATE -Wshadow
  PRIVATE -pedantic
  PRIVATE -Werror
  PRIVATE -Weffc++
  PRIVATE -O2)

find_package(Threads REQUIRED)
target_link_libraries(tester Threads::Threads)
