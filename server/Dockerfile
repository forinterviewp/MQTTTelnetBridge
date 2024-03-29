FROM ubuntu:22.04

RUN apt update && apt install -y git llvm clang cmake google-mock libgtest-dev

# Compile gmock
WORKDIR /usr/src/gmock
RUN cmake CMakeLists.txt && make

# Compile gtest
WORKDIR /usr/src/gtest
RUN cmake CMakeLists.txt && make

# Building the Paho C library
WORKDIR /usr/src/pahomqtt
RUN git clone https://github.com/eclipse/paho.mqtt.c.git && \
    cd paho.mqtt.c && \
    git checkout v1.3.8 && \
    cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target install

# Building the Paho C++ library after
RUN git clone https://github.com/eclipse/paho.mqtt.cpp && \
    cd paho.mqtt.cpp && \
    cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_ENABLE_TESTING=OFF -DPAHO_WITH_SSL=OFF -DPAHO_HIGH_PERFORMANCE=ON && \
    cmake --build build/ --target install

RUN ldconfig

ADD ./src /mqtt_bridge_src
WORKDIR /mqtt_bridge_src

RUN mkdir ./build && \
    cd ./build && \
    cmake .. && make && \
    cp ./server/server /usr/local/bin/mqtt_bridge_server

ENTRYPOINT [ "mqtt_bridge_server", "1234" ]

