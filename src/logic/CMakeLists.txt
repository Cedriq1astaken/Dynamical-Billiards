cmake_minimum_required(VERSION 3.30)
project(logic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -funroll-loops")
include_directories(.)

add_executable(logic
    Billiard.cpp
    Billiard.h
    Vec2.h
    SinaiBilliard.cpp
    SinaiBilliard.h
    Schrodinger.cpp
    SinaiBilliard.h
    Utils.h
    main.cpp
)
