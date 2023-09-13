FROM ubuntu:22.04

RUN apt update && apt install -y llvm clang cmake google-mock libgtest-dev

# Compile gmock
WORKDIR /usr/src/gmock
RUN cmake CMakeLists.txt && make

# Compile gtest
WORKDIR /usr/src/gtest
RUN cmake CMakeLists.txt && make

# TODO move git:
# Building the Paho C library
WORKDIR /usr/src/pahomqtt
RUN apt install -y git
RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && \
    git checkout v1.3.8 && \
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target install

# Building the Paho C++ library after
WORKDIR /usr/src/pahomqtt
RUN git clone https://github.com/eclipse/paho.mqtt.cpp && \
    cd paho.mqtt.cpp && \
    cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_ENABLE_TESTING=OFF -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target install

RUN ldconfig

WORKDIR /src

# docker build -t mqttbridge_dev -f ./develop.dockerfile .
# docker run -it --rm -v $(pwd -P)/src:/src:ro --name mqttbridge_dev mqttbridge_dev

