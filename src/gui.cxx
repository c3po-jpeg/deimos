#include "headers/gui.hxx"
#include "../renderer/headers/core.hxx"
#include "../renderer/headers/renderpass.hxx"
#include "../renderer/headers/swapchain.hxx"

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl3.h"
#include "../external/imgui/imgui_impl_vulkan.h"

Gui::Gui(Core &core, SDL_Window *window, RenderPass *renderPass, Swapchain *swapchain)
    : m_core(core)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    (void)m_io;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForVulkan(window);

    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
        {VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    for (VkDescriptorPoolSize &pool_size : pool_sizes)
        pool_info.maxSets += pool_size.descriptorCount;
    pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    vkCreateDescriptorPool(m_core.getDevice(), &pool_info, nullptr, &m_descPool);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_core.getInstance();
    init_info.PhysicalDevice = m_core.getPhysicaldevice();
    init_info.Device = m_core.getDevice();
    init_info.QueueFamily = m_core.getGraphicsFamilyIndex();
    init_info.Queue = m_core.getGraphicsQueue();
    init_info.PipelineCache = VK_NULL_HANDLE;
    init_info.DescriptorPool = m_descPool;
    init_info.MinImageCount = static_cast<uint32_t>(swapchain->getImageViews().size());
    init_info.ImageCount = static_cast<uint32_t>(swapchain->getImageViews().size());
    init_info.Allocator = nullptr;
    init_info.PipelineInfoMain.RenderPass = renderPass->getHandle();
    ImGui_ImplVulkan_Init(&init_info);
}

Gui::~Gui() 
{
    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(m_core.getDevice(), m_descPool, nullptr);
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void Gui::newFrame()
{
    ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
}

void Gui::render()
{
    ImGui::Render();
}

