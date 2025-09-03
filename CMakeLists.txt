cmake_minimum_required(VERSION 3.30)
project(Dynamical_Billiards)

set(CMAKE_CXX_STANDARD 14)
find_package(raylib REQUIRED)
find_package(Spectra REQUIRED)
find_package(Eigen3 REQUIRED)

include_directories(src/logic)
include_directories(src/miscellaneous)

add_executable(Dynamical_Billiards
    src/writer/writer.cpp
    src/writer/writer.h
    src/logic/Billiard.cpp
    src/logic/Billiard.h
    src/logic/Schrodinger.cpp
    src/logic/Schrodinger.h
    src/logic/SinaiBilliard.cpp
    src/logic/SinaiBilliard.h
    src/miscellaneous/Utils.h
    src/miscellaneous/Vec2.h
    src/main.cpp)

target_link_libraries(Dynamical_Billiards PRIVATE raylib Spectra::Spectra Eigen3::Eigen)