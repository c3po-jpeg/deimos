# ---------------------------------------------------------------------------
# math/CMakeLists.txt
# Builds your custom math library as a static library target: deimos_math
# ---------------------------------------------------------------------------

# Glob is convenient here because you control this directory and will re-run
# CMake when you add new files.  For larger projects prefer explicit lists.
file(GLOB MATH_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cxx")
file(GLOB MATH_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/headers/*.hxx")

add_library(deimos_math STATIC ${MATH_SOURCES} ${MATH_HEADERS})

# PUBLIC: anything that links deimos_math automatically gets this include
# path, so they can write #include "vec3.hxx" without extra setup.
target_include_directories(deimos_math
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

target_compile_features(deimos_math PUBLIC cxx_std_17)