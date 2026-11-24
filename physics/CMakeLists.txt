# ---------------------------------------------------------------------------
# physics/CMakeLists.txt
# ---------------------------------------------------------------------------

# Physics library - currently empty, will be populated with physics code
add_library(deimos_physics STATIC
    rigidbody.cxx
    physicsworld.cxx
    collision.cxx
)

target_include_directories(deimos_physics
    INTERFACE
        headers
        ../math/headers
)
target_link_libraries(deimos_physics
    PUBLIC
        deimos_math
)