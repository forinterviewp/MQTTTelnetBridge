cmake_minimum_required(VERSION 3.1)

set(TARGET "server")
add_executable(server server.cpp mqtt_client.cpp)

target_include_directories(${TARGET} PUBLIC "${PROJECT_SOURCE_DIR}/server.cpp")

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
