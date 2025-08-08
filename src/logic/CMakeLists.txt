cmake_minimum_required(VERSION 3.30)
project(logic)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(logic
    Billiard.cpp
    Billiard.h
    Vec2.h
    main.cpp
)
