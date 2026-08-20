cmake_minimum_required(VERSION 3.25)
project(LoomInstallSmokeTest LANGUAGES CXX)

# Consumes the INSTALLED engine, not the source tree. This project deliberately
# names no third-party library: if it builds, the engine is carrying its own
# dependencies correctly and a game author needs to know nothing about them.
find_package(Loom REQUIRED COMPONENTS Render2D)

add_executable(LoomInstallSmokeTest Source/main.cpp)
target_link_libraries(LoomInstallSmokeTest PRIVATE Loom::Core Loom::Render2D)
