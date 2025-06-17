cmake_minimum_required(VERSION 3.20)
project(Loom LANGUAGES CXX)

# C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Loom (Engine) subproject
add_subdirectory(Loom)