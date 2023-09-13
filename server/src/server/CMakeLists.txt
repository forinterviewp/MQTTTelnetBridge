cmake_minimum_required(VERSION 3.1)

set(TARGET "server")

find_package(PahoMqttCpp REQUIRED)
find_package(Threads REQUIRED)

add_executable(${TARGET} server.cpp mqtt_client.cpp)

target_compile_features(${TARGET} PUBLIC cxx_std_14)

target_link_libraries(${TARGET}
  PRIVATE paho-mqttpp3
  PRIVATE paho-mqtt3a
  PRIVATE Threads::Threads)

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
