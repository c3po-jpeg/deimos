# ---------------------------------------------------------------------------
# renderer/CMakeLists.txt
# ---------------------------------------------------------------------------

add_library(deimos_renderer STATIC
    buffer.cxx
    core.cxx
    descriptors.cxx
    pipeline.cxx
    renderer.cxx
    renderpass.cxx
    shader.cxx
    shadowmap.cxx
    swapchain.cxx
    texture.cxx
    utils.cxx
    vertex.cxx
)

target_include_directories(deimos_renderer
    PUBLIC
        headers
)

target_link_libraries(deimos_renderer
    PUBLIC
        Vulkan::Vulkan
        SDL3::SDL3
)