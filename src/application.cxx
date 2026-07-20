#include "headers/application.hxx"
#include "headers/scene.hxx"
#include "headers/gui.hxx"
#include "headers/camera.hxx"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <iostream>
#include <vulkan/vulkan.h>

#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_sdl3.h"
#include "../external/imgui/imgui_impl_vulkan.h"

#include "../renderer/headers/core.hxx"
#include "../renderer/headers/descriptors.hxx"
#include "../renderer/headers/drawable.hxx"
#include "../renderer/headers/pipeline.hxx"
#include "../renderer/headers/renderer.hxx"
#include "../renderer/headers/renderpass.hxx"
#include "../renderer/headers/shader.hxx"
#include "../renderer/headers/shadowmap.hxx"
#include "../renderer/headers/swapchain.hxx"
#include "../renderer/headers/ubo.hxx"
#include "../renderer/headers/vertex.hxx"

Application::Application(const std::string &title, int width, int height)
	: m_width(width), m_height(height)
{
	try
	{
		initializeSDL(title);
		initializeVulkan();
		initImgui();
	}
	catch (const std::exception &e)
	{
		shutdownImgui();
		shutdownVulkan();
		shutdownSDL();
		throw;
	}
}

Application::~Application()
{
	shutdownImgui();
	shutdownVulkan();
	shutdownSDL();
	
}

void Application::initializeSDL(const std::string &title)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		throw std::runtime_error(std::string("Failed to initialize SDL3: ") + SDL_GetError());
	}

	if (!SDL_Vulkan_LoadLibrary(nullptr))
		throw std::runtime_error(std::string("Failed to load Vulkan: ") + SDL_GetError());

	m_window = SDL_CreateWindow(title.c_str(), m_width, m_height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	if (!m_window)
	{
		SDL_Quit();
		throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
	}

	std::cout << "Window created: " << m_width << "x" << m_height << std::endl;
}

void Application::initializeVulkan()
{
	m_core       = std::make_unique<Core>(m_window);
	m_swapchain  = std::make_unique<Swapchain>(*m_core, m_window);
	m_renderPass = std::make_unique<RenderPass>(*m_core, m_swapchain->getFormat(), m_swapchain->getDepthFormat());
	m_shadowMap  = std::make_unique<ShadowMap>(*m_core);

	// ---- Pipeline -------------------------------------------------------
	Shader vertShader(*m_core, "build/shaders/shader.vert.spv");
	Shader fragShader(*m_core, "build/shaders/shader.frag.spv");

	VkPushConstantRange pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushRange.offset     = 0;
	pushRange.size       = sizeof(Mat4x4);

	auto attribDescs = Vertex3D::getAttributeDescriptions();

	m_globalDescriptor = std::make_unique<GlobalDescriptor>(*m_core, MAX_FRAMES_IN_FLIGHT, *m_shadowMap);
	MaterialDescriptor::createLayout(m_core->getDevice());
	std::cout << "material descriptor created" << std::endl;

	PipelineConfig pipelineConfig{
		.core                  = *m_core,
		.renderPass            = m_renderPass->getHandle(),
		.swapChainExtent       = m_swapchain->getExtent(),
		.vertShader            = &vertShader,
		.fragShader            = &fragShader,
		.globalDescLayout      = m_globalDescriptor->getLayout(),
		.materialDescLayout    = MaterialDescriptor::getLayout(),
		.bindingDescriptions   = {Vertex3D::getBindingDescription()},
		.attributeDescriptions = {attribDescs.begin(), attribDescs.end()},
		.pushConstantRanges    = {pushRange},
	};
	m_pipeline = std::make_unique<Pipeline>(pipelineConfig);

	// ---- Wireframe Pipeline
	// -------------------------------------------------------

	pipelineConfig.wireframe = true;
	pipelineConfig.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	m_wireframePipeline = std::make_unique<Pipeline>(pipelineConfig);

	// ---- Renderer -------------------------------------------------------
	RendererConfig rendererConfig{
		.core                = *m_core,
		.renderPass          = m_renderPass->getHandle(),
		.swapChainExtent     = m_swapchain->getExtent(),
		.swapChainImageViews = m_swapchain->getImageViews(),
		.depthImageView      = m_swapchain->getDepthView(),
		.globalDescriptor    = *m_globalDescriptor,
		//.materialDescriptor  = *m_materialDescriptor
	};
	m_renderer = std::make_unique<Renderer>(rendererConfig);
}

void Application::initImgui()
{
	m_gui = std::make_unique<Gui>(*m_core, m_window, m_renderPass.get(), m_swapchain.get());
}

void Application::shutdownImgui()
{
	m_gui.reset();
}

void Application::shutdownSDL()
{
	if (m_window)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
	SDL_Vulkan_UnloadLibrary();
	SDL_Quit();
}

void Application::shutdownVulkan()
{
	// Destroy command buffers FIRST (they reference other resources)
	m_renderer.reset();
	if (m_core)
	{
		MaterialDescriptor::destroyLayout(m_core->getDevice());
	}
	// Then destroy the resources they were referencing
	m_pipeline.reset();
	m_wireframePipeline.reset();
	m_globalDescriptor.reset();

	m_shadowMap.reset();
	m_renderPass.reset();
	m_swapchain.reset();
	m_core.reset();
}

void Application::run(Scene &scene)
{
	uint64_t lastTicks = SDL_GetTicks();
	float deltaTime = 0.0f;
	bool running = true;

	while (true)
	{
		const uint64_t now = SDL_GetTicks();
		deltaTime = static_cast<float>(now - lastTicks) / 1000.0f;
		lastTicks = now;

		if (!pollEvents())
			break;

		// Handle window resize if needed
		if (m_windowResized)
		{
			recreateSwapchain();
			m_windowResized = false;
		}

		handleInput(scene, deltaTime);
		updateScene(scene, deltaTime);

		m_gui->newFrame();
		debugWindow(deltaTime);
		m_gui->render();

		renderFrame(scene);
	}

	// Ensure GPU is idle before Scene is destroyed
	// (Scene's buffers are still being referenced by command buffers)
	if (m_core)
		vkDeviceWaitIdle(m_core->getDevice());
}

bool Application::pollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			std::cout << "Quit event received, exiting main loop." << std::endl;
			return false;

		case SDL_EVENT_WINDOW_RESIZED:
		{
			m_width         = event.window.data1;
			m_height        = event.window.data2;
			m_windowResized = true;
			std::cout << "Window resized to: " << m_width << "x" << m_height << std::endl;
		}
		break;

		case SDL_EVENT_KEY_DOWN:
			switch (event.key.key)
			{
			case SDLK_ESCAPE:
				return false;
			case SDLK_TAB:
			{
				SDL_SetWindowRelativeMouseMode(
					m_window, !SDL_GetWindowRelativeMouseMode(m_window));
				break;
			}
			default:
				break;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
		{
			// Get mouse motion
			if (SDL_GetWindowRelativeMouseMode(m_window))
			{
				m_mouseX = static_cast<int>(event.motion.xrel);
				m_mouseY = static_cast<int>(event.motion.yrel);
			}
		}

		default:
			break;
		}
	}
	return true;
}

void Application::handleInput(Scene &scene, float deltaTime)
{
	const bool *keys = SDL_GetKeyboardState(nullptr);

	scene.handleInput(deltaTime, keys, m_mouseX, m_mouseY);
	//
	m_mouseX = 0;
	m_mouseY = 0;
}

void Application::updateScene(Scene &scene, float deltaTime)
{
	scene.update(deltaTime, static_cast<float>(m_width) / static_cast<float>(m_height));
}

void Application::debugWindow(float deltaTime)
{
	ImGui::Begin("Debug");
	ImGui::Text("Frame time: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
}

void Application::renderFrame(Scene &scene)
{
	m_renderer->clearColor(0.2, 0.3, 0.6);

	// Compute light space matrix
	auto light = scene.getLight();
	light.lightSpaceMatrix = m_shadowMap->computeLightSpaceMatrix(Vector3f(light.direction.x, light.direction.y, light.direction.z));

	uint32_t frameIndex = m_renderer->getFrame(m_swapchain->getHandle(), m_swapchain->getExtent());

	m_renderer->beginRecording();

	// ---- Shadow render pass
	{
		m_shadowMap->beginRenderpass(m_renderer->getCommandBuffer());

		for (const auto &drawable : scene.getDrawables())
		{
			m_shadowMap->drawShadow(m_renderer->getCommandBuffer(), drawable, light.lightSpaceMatrix);
		}

		m_shadowMap->endRenderpass(m_renderer->getCommandBuffer());
	}

	// ---- Main render pass
	{
		m_renderer->beginRenderPass(m_renderPass->getHandle(), frameIndex, m_swapchain->getExtent());

		if (m_wireframeMode)
		{
			m_renderer->bindPipeline(*m_wireframePipeline);
			m_renderer->bindGlobalDescriptors(scene.getCamera().getUBO(), light, m_wireframePipeline->getLayout());
			for (const auto &drawable : scene.getDrawables())
			{
				m_renderer->bindMaterial(drawable.material, m_wireframePipeline->getLayout());
				m_renderer->draw(drawable, *m_wireframePipeline);
			}
		}
		else
		{
			m_renderer->bindPipeline(*m_pipeline);
			m_renderer->bindGlobalDescriptors(scene.getCamera().getUBO(), light, m_pipeline->getLayout());
			for (const auto &drawable : scene.getDrawables())
			{
				m_renderer->bindMaterial(drawable.material, m_pipeline->getLayout());
				// std::cout << drawable.material.useChecker << std::endl;
				m_renderer->draw(drawable, *m_pipeline);
			}

			// ---- ImGui ----
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderer->getCommandBuffer());
		}

		m_renderer->endRenderPass();
	}

	m_renderer->endRecording();
	m_renderer->presentFrame(m_swapchain->getHandle(), frameIndex);
}

void Application::recreateSwapchain()
{
	// Wait for device to idle before recreating resources
	vkDeviceWaitIdle(m_core->getDevice());

	// MaterialDescriptor::destroyLayout(m_core->getDevice());

	// Destroy resources that depend on swapchain in reverse order
	m_renderer.reset();
	m_pipeline.reset();
	// m_globalDescriptor.reset();
	m_renderPass.reset();
	m_swapchain.reset();

	// Recreate swapchain and dependent resources
	m_swapchain  = std::make_unique<Swapchain>(*m_core, m_window);
	m_renderPass = std::make_unique<RenderPass>(*m_core, m_swapchain->getFormat(), m_swapchain->getDepthFormat());

	// ---- Pipeline -------------------------------------------------------
	Shader vertShader(*m_core, "build/shaders/shader.vert.spv");
	Shader fragShader(*m_core, "build/shaders/shader.frag.spv");

	VkPushConstantRange pushRange{};
	pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushRange.offset     = 0;
	pushRange.size       = sizeof(Mat4x4);

	auto attribDescs = Vertex3D::getAttributeDescriptions();

	// m_globalDescriptor = std::make_unique<GlobalDescriptor>(*m_core,
	// MAX_FRAMES_IN_FLIGHT, *m_shadowMap);
	PipelineConfig pipelineConfig{
		.core                  = *m_core,
		.renderPass            = m_renderPass->getHandle(),
		.swapChainExtent       = m_swapchain->getExtent(),
		.vertShader            = &vertShader,
		.fragShader            = &fragShader,
		.globalDescLayout      = m_globalDescriptor->getLayout(),
		.materialDescLayout    = MaterialDescriptor::getLayout(),
		.bindingDescriptions   = {Vertex3D::getBindingDescription()},
		.attributeDescriptions = {attribDescs.begin(), attribDescs.end()},
		.pushConstantRanges    = {pushRange},
	};
	m_pipeline = std::make_unique<Pipeline>(pipelineConfig);

	// ---- Wireframe Pipeline
	// -------------------------------------------------------

	pipelineConfig.wireframe = true;
	pipelineConfig.topology  = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	m_wireframePipeline      = std::make_unique<Pipeline>(pipelineConfig);

	// ---- Renderer -------------------------------------------------------
	RendererConfig rendererConfig{
		.core                = *m_core,
		.renderPass          = m_renderPass->getHandle(),
		.swapChainExtent     = m_swapchain->getExtent(),
		.swapChainImageViews = m_swapchain->getImageViews(),
		.depthImageView      = m_swapchain->getDepthView(),
		.globalDescriptor    = *m_globalDescriptor,
		//.materialDescriptor  = *m_materialDescriptor
	};
	m_renderer = std::make_unique<Renderer>(rendererConfig);

	std::cout << "Swapchain recreated for new size: " << m_width << "x" << m_height << std::endl;
}
