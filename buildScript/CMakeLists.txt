# CMake Parent File Requirements
cmake_minimum_required(VERSION 3.28...4.2)

# Project Definition
project(
        SimpleCPP_BuildScript
        DESCRIPTION "A script in C designed to handle runtime dependencies and static registration"
        VERSION 1.0.0
        LANGUAGES C CXX
)

add_executable(sbs
        json.h
        main.cpp
)

target_compile_features(sbs PUBLIC "cxx_std_17")
