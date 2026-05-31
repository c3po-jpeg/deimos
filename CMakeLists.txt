cmake_minimum_required(VERSION 3.16)
project(Enceladus)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED CONFIG)

# ---------------------------------------------------------------------------
# Subdirectories — order matters: external libs before src so their targets
# exist when src/CMakeLists.txt calls target_link_libraries.
# ---------------------------------------------------------------------------
add_subdirectory(external)
add_subdirectory(math)
add_subdirectory(renderer)
add_subdirectory(physics)
add_subdirectory(geometry)
add_subdirectory(src)