#pragma once
#include "Rendering/PresentationObjects.h"
#include "Rendering/RENDER_CORE.h"
#include "Rendering/Shader.h"
#include "Rendering/SyncObjects.h"

#include "Firefly/Window.h"

namespace Firefly {
namespace Rendering {

class RenderBackend {
  public:
    void Init(Window window) {
        _current_frame = 0;
        _CreateInstance();
        _SetupDebugLayer();
        _CreateSurface(window);
        _PickPhysicalDevice();
        _CreateLogicalDevice();
        _CreateSwapchain(window);
        _CreateCommandPoolAndBuffers();
        _CreateSyncObjects();
        _CreateRenderPass();
        _CreatePresentationObjects();
        _CreatePipeline();
        _RecordRenderPass();
    }

    void OnUpdate();

    ~RenderBackend() { _CleanUp(); }

  private:
    void _CreateInstance();
    void _QueryRequiredExtensions();

    void _CreateSurface(Window window);
    /* === Physical Devices === */
    void _PickPhysicalDevice();
    bool _IsDeviceSuitable(VkPhysicalDevice dev); /* TODO */
    void _FindQueueFamilies(VkPhysicalDevice dev, uint32* graphics_index,
                            uint32* presentation_index);
    void _CreateLogicalDevice();
    /* ===  ===  === === === */
    /* === Swapchain === */
    void _CreateSwapchain(Window window);
    void _ChooseSwapchainSurfaceFormat(std::vector<VkSurfaceFormatKHR>& format);
    void _ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR>& present_mode);
    void _QuerySwapchainSupport(VkSurfaceCapabilitiesKHR*        available_capabilities,
                                std::vector<VkSurfaceFormatKHR>* available_formats,
                                std::vector<VkPresentModeKHR>* available_present_modes);
    /* ===  ===  === === === */
    /* === Commands === */
    void _CreateCommandPoolAndBuffers();
    void _BeginCommandRecording(VkCommandBufferUsageFlags usage,
                                VkCommandBuffer&          target);
    void _EndCommandRecording(VkCommandBuffer& target);
    void _EndCommandRecordingAndSubmit(VkCommandBuffer& target);
    void _CreateSyncObjects();
    /* ===  ===  === === === */
    /* === Pipeline === */
    void _CreateRenderPass();
    void _RecordRenderPass();
    void _CreatePipeline();
    /* ===  ===  === === === */
    /* === Presentation === */
    void _CreatePresentationObjects();
    /* ===  ===  === === === */
    /* === Debug Layer === */
    void _SetupDebugLayer();
    /* ===  ===  === === === */
    void _CleanUp();

  private:
    VkInstance   _instance;
    VkSurfaceKHR _surface;
    /* ===  ===  === === === === === === */
    VkPhysicalDevice _physical_dev;
    VkDevice         _device;
    VkQueue          _graphics_queue;
    VkQueue          _present_queue;
    /* ===  ===  === === === === === === */
    VkSwapchainKHR       _swapchain;
    std::vector<VkImage> _swapchain_images;
    VkFormat             _swapchain_img_format;
    VkExtent2D           _swapchain_extent;
    /* ===  ===  === === === === === === */
    VkCommandPool                _command_pool;
    std::vector<VkCommandBuffer> _render_command_buffers;
    /* ===  ===  === === === === === === */
    std::vector<Presentation::FramebufferAttachment> _color_attachments;
    Presentation::FramebufferAttachment              _depth_attachment;
    std::vector<Presentation::Framebuffer>           _swapchain_framebuffers;
    Presentation::Framebuffer                        _depth_framebuffer;
    /* ===  ===  === === === === === === */
    std::vector<VkRenderPass> _renderpass;
    VkPipeline                _graphics_pipeline;
    VkPipelineLayout          _graphics_pipeline_layout;
    /* ===  ===  === === === === === === */
    std::vector<VkSemaphore>
        _acquired_semaphore; /* Signaling that an Image is acquired */
    std::vector<VkSemaphore>
           _released_semaphore; /* Signaling that the Acquired Image is released */
    std::vector<VkFence> _in_flight_fences;
    uint32 _current_frame;
    /* ===  ===  === === === === === === */
    VkDebugUtilsMessengerEXT _debug_messenger;
};

/* Functions for the debug layer */

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data);
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                   VkDebugUtilsMessengerEXT     messenger,
                                   const VkAllocationCallbacks* pAllocator);

} // namespace Rendering
} // namespace Firefly