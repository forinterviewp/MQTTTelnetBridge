FROM ubuntu:22.04

RUN apt update && apt install -y llvm clang cmake google-mock libgtest-dev

# Compile gmock
WORKDIR /usr/src/gmock
RUN cmake CMakeLists.txt && make

# Compile gtest
WORKDIR /usr/src/gtest
RUN cmake CMakeLists.txt && make

# TODO:
# Building the Paho C library
WORKDIR /usr/src/pahomqtt
RUN apt install -y git
RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && \
    git checkout v1.3.8 && \
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target install

WORKDIR /src

