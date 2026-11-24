# ---------------------------------------------------------------------------
# geometry/CMakeLists.txt
# ---------------------------------------------------------------------------

add_library(deimos_geometry STATIC
    cube.cxx
    cubesphere.cxx
    icosphere.cxx
    plane.cxx
    shape.cxx
)

target_include_directories(deimos_geometry
    PUBLIC
        headers
        ../math/headers
        ../renderer/headers
)

target_link_libraries(deimos_geometry
    PUBLIC
        deimos_math
        deimos_renderer
)