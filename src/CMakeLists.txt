add_executable(deimos
    main.cxx
    scene.cxx
    application.cxx
    camera.cxx
    gui.cxx
)

target_link_libraries(deimos
    PRIVATE
        Vulkan::Vulkan
        SDL3::SDL3
        deimos_math   # your custom math library
        deimos_renderer
        deimos_geometry
        deimos_physics
        imgui            # Dear ImGui (Vulkan + SDL3 backends)
)

target_include_directories(deimos
    PRIVATE
        ../renderer/headers
        ../physics/headers
        ../geometry/headers
        # deimos_math and imgui expose their own include dirs via PUBLIC
        # in their own CMakeLists, so no need to repeat them here.
)
