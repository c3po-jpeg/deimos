# ---------------------------------------------------------------------------
# geometry/CMakeLists.txt
# ---------------------------------------------------------------------------

add_library(enceladus_geometry STATIC
    cube.cxx
    cubesphere.cxx
    icosphere.cxx
    plane.cxx
    shape.cxx
)

target_include_directories(enceladus_geometry
    PUBLIC
        headers
        ../math/headers
        ../renderer/headers
)

target_link_libraries(enceladus_geometry
    PUBLIC
        enceladus_math
        enceladus_renderer
)