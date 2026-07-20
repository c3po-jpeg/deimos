#ifndef GUI_HXX
#define GUI_HXX

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

class Core;

class Gui
{
public:
    Gui(Core &core, SDL_Window *window, class RenderPass *renderPass, class Swapchain *swapchain);
    ~Gui();

    void newFrame();
    void render();

private:
    Core &m_core;

    struct ImGuiIO  *m_io = nullptr;
    VkDescriptorPool m_descPool = VK_NULL_HANDLE;
};

#endif