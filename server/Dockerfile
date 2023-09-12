FROM ubuntu:22.04

RUN apt update && apt install -y llvm clang cmake google-mock libgtest-dev

# Compile gmock
WORKDIR /usr/src/gmock
RUN cmake CMakeLists.txt && make

# Compile gtest
WORKDIR /usr/src/gtest
RUN cmake CMakeLists.txt && make

WORKDIR /src

