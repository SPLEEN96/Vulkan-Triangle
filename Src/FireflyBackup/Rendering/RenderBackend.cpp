#include "Rendering/RenderBackend.h"

#include "PCH_CORE.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Firefly {
namespace Rendering {

std::vector<const char*> extensions;
std::vector<const char*> device_extensions    = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
std::vector<const char*> debug_layers         = {"VK_LAYER_LUNARG_standard_validation"};
const uint32             MAX_FRAMES_IN_FLIGHT = 4;

void RenderBackend::OnUpdate() {
    vkWaitForFences(_device, 1, &_in_flight_fences[_current_frame], VK_TRUE,
                    std::numeric_limits<uint64>::max());
    vkResetFences(_device, 1, &_in_flight_fences[_current_frame]);

    uint32 image_index = 0;

    VK_ASSERT(vkAcquireNextImageKHR(
                  _device, _swapchain, std::numeric_limits<uint64>::max(),
                  _acquired_semaphore[_current_frame], VK_NULL_HANDLE, &image_index),
              "Failed to acquire next Swapchain Image.");

    VkPipelineStageFlags submit_stage_mask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info         = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.waitSemaphoreCount   = 1;
    submit_info.pWaitSemaphores      = &_acquired_semaphore[_current_frame];
    submit_info.pWaitDstStageMask    = &submit_stage_mask;
    submit_info.commandBufferCount   = 1;
    submit_info.pCommandBuffers      = &_render_command_buffers[image_index];
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores    = &_released_semaphore[_current_frame];

    VK_ASSERT(vkQueueSubmit(_graphics_queue, 1, &submit_info,
                            _in_flight_fences[_current_frame]),
              "Failed to submit the Drawing Command Buffer to the Graphics Queue.");

    VkPresentInfoKHR present_info   = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores    = &_released_semaphore[_current_frame];
    present_info.swapchainCount     = 1;
    present_info.pSwapchains        = &_swapchain;
    present_info.pImageIndices      = &image_index;

    vkQueuePresentKHR(_present_queue, &present_info);

    _current_frame = (_current_frame + 1) % (MAX_FRAMES_IN_FLIGHT);
}

void RenderBackend::_CreateInstance() {
    VkApplicationInfo app_info  = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.pApplicationName   = "Firefly Rendering Engine";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName        = "Firefly";
    app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion         = VK_API_VERSION_1_1;

    _QueryRequiredExtensions();

    VkInstanceCreateInfo create_info    = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo        = &app_info;
    create_info.enabledExtensionCount   = static_cast<uint32>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount       = static_cast<uint32>(debug_layers.size());
    create_info.ppEnabledLayerNames     = debug_layers.data();

    VK_ASSERT(vkCreateInstance(&create_info, nullptr, &_instance),
              "Failed to create Vulkan Instance.");
}

void RenderBackend::_QueryRequiredExtensions() {
    uint32       ext_count;
    const char** glfw_exts = glfwGetRequiredInstanceExtensions(&ext_count);
    extensions.insert(extensions.end(), glfw_exts, glfw_exts + ext_count);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void RenderBackend::_CreateSurface(Window window) {
    VK_ASSERT(glfwCreateWindowSurface(
                  _instance, static_cast<GLFWwindow*>(window.GetPlatformWindow()),
                  nullptr, &_surface),
              "Failed to create Vulkan Surface.");
}

/* === === === === === === Physical Devices === === === === === === */
void RenderBackend::_PickPhysicalDevice() {
    uint32 dev_count = 0;
    vkEnumeratePhysicalDevices(_instance, &dev_count, nullptr);
    FFLY_ASSERT(dev_count, "Failed to find GPUs with Vulkan support.");

    VkPhysicalDevice device_list[dev_count];
    vkEnumeratePhysicalDevices(_instance, &dev_count, device_list);

    for (VkPhysicalDevice dev : device_list) {
        if (_IsDeviceSuitable(dev)) {
            _physical_dev = dev;
        }
    }

    FFLY_ASSERT(_physical_dev, "Failed to find a suitable GPU.");

    VkPhysicalDeviceProperties dev_properties = {};
    vkGetPhysicalDeviceProperties(_physical_dev, &dev_properties);
    std::cout << "Device used:" << dev_properties.deviceName << std::endl
              << "API_VERSION:" << dev_properties.apiVersion << std::endl;
}

bool RenderBackend::_IsDeviceSuitable(VkPhysicalDevice dev) {
    VkPhysicalDeviceProperties dev_properties;
    VkPhysicalDeviceFeatures   dev_features;
    vkGetPhysicalDeviceProperties(dev, &dev_properties);
    vkGetPhysicalDeviceFeatures(dev, &dev_features);

    uint32 ext_count;
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &ext_count, nullptr);
    std::vector<VkExtensionProperties> dev_available_ext(ext_count);
    vkEnumerateDeviceExtensionProperties(dev, nullptr, &ext_count,
                                         dev_available_ext.data());

    return true;
}

void RenderBackend::_FindQueueFamilies(VkPhysicalDevice dev, uint32* graphics_index,
                                       uint32* presentation_index) {
    if (graphics_index == nullptr && presentation_index == nullptr) {
        return;
    }
    if (graphics_index != nullptr) {
        *graphics_index = -1;
    }
    if (presentation_index != nullptr) {
        *graphics_index = *presentation_index = -1;
    }
    bool found_graphics     = false;
    bool found_presentation = false;

    uint32 queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_dev, &queue_family_count,
                                             nullptr);
    FFLY_ASSERT(queue_family_count, "Failed to find Queues on Physical Device.");

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(_physical_dev, &queue_family_count,
                                             queue_families.data());

    for (uint32 i = 0; i < queue_families.size(); i++) {
        if (graphics_index != nullptr) {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                *graphics_index = i;
                found_graphics  = true;
            }
        } else {
            found_graphics = true;
        }
        if (presentation_index != nullptr) {
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(_physical_dev, i, _surface,
                                                 &present_support);
            if (present_support) {
                *presentation_index = i;
                found_presentation  = true;
            }
        } else {
            found_presentation = true;
        }

        if (found_graphics && found_presentation) {
            break;
        }
    }
}

void RenderBackend::_CreateLogicalDevice() {
    uint32 graphics_family_index, present_family_index;
    _FindQueueFamilies(_physical_dev, &graphics_family_index, &present_family_index);
    std::set<uint32> indices = {graphics_family_index, present_family_index};

    float queue_priority = 1.f;

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    for (auto index : indices) {
        VkDeviceQueueCreateInfo create_info = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
        create_info.queueFamilyIndex = index;
        create_info.queueCount       = 1;
        create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(create_info);
    }

    VkPhysicalDeviceFeatures dev_features = {};
    dev_features.samplerAnisotropy        = VK_TRUE;

    VkDeviceCreateInfo create_info      = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    create_info.queueCreateInfoCount    = static_cast<uint32>(queue_create_infos.size());
    create_info.pQueueCreateInfos       = queue_create_infos.data();
    create_info.pEnabledFeatures        = &dev_features;
    create_info.enabledExtensionCount   = static_cast<uint32>(device_extensions.size());
    create_info.ppEnabledExtensionNames = device_extensions.data();
    create_info.enabledLayerCount       = static_cast<uint32>(debug_layers.size());
    create_info.ppEnabledLayerNames     = debug_layers.data();

    VK_ASSERT(vkCreateDevice(_physical_dev, &create_info, nullptr, &_device),
              "Failed to create Logical Device.");

    vkGetDeviceQueue(_device, graphics_family_index, 0, &_graphics_queue);
    vkGetDeviceQueue(_device, present_family_index, 0, &_present_queue);
}
/* ===  ===  === === === === === === === ===*/

/* === === === === === === Swapchain === === === === === === */
void RenderBackend::_CreateSwapchain(Window window) {
    _swapchain_extent = {window.Data().Width, window.Data().Height};
    VkSurfaceCapabilitiesKHR        capabilities;
    std::vector<VkSurfaceFormatKHR> format;
    std::vector<VkPresentModeKHR>   present_mode;
    _QuerySwapchainSupport(&capabilities, nullptr, nullptr);
    _ChooseSwapchainSurfaceFormat(format);
    _ChooseSwapchainPresentMode(present_mode);
    
    uint32 graphics_family_index, present_family_index;
    _FindQueueFamilies(_physical_dev, &graphics_family_index, &present_family_index);
    std::vector<uint32> indices = {graphics_family_index, present_family_index};

    uint32 image_count = 2;

    VkSwapchainCreateInfoKHR create_info = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    create_info.surface                  = _surface;
    create_info.minImageCount            = image_count;
    create_info.imageFormat              = format[0].format;
    create_info.imageColorSpace          = format[0].colorSpace;
    create_info.imageExtent.width        = _swapchain_extent.width;
    create_info.imageExtent.height       = _swapchain_extent.height;
    create_info.imageArrayLayers         = 1;
    create_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (graphics_family_index != present_family_index) {
        create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices   = indices.data();
    } else {
        create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices   = nullptr;
    }
    create_info.presentMode    = present_mode[0];
    create_info.preTransform   = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.clipped        = VK_TRUE;
    create_info.oldSwapchain   = VK_NULL_HANDLE;

    VK_ASSERT(vkCreateSwapchainKHR(_device, &create_info, nullptr, &_swapchain),
              "Failed to create Swapchain.");

    vkGetSwapchainImagesKHR(_device, _swapchain, &image_count, nullptr);
    _swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(_device, _swapchain, &image_count, _swapchain_images.data());

    _swapchain_img_format = format[0].format;
}

void RenderBackend::_ChooseSwapchainSurfaceFormat(
    std::vector<VkSurfaceFormatKHR>& formats) {
    _QuerySwapchainSupport(nullptr, &formats, nullptr);

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        formats[0] = {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        return;
    }
    for (const auto& format : formats) {
        if (((format.format == VK_FORMAT_R16G16B16A16_SFLOAT) ||
             (format.format == VK_FORMAT_B8G8R8A8_UNORM)) &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            formats.resize(1);
            formats[0] = format;
            return;
        }
    }
    formats.resize(1);
}

void RenderBackend::_ChooseSwapchainPresentMode(
    std::vector<VkPresentModeKHR>& present_mode) {
    _QuerySwapchainSupport(nullptr, nullptr, &present_mode);

    for (size_t i = 0; i < present_mode.size(); i++) {
        if (present_mode[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode.resize(1);
            present_mode[0] = VK_PRESENT_MODE_MAILBOX_KHR;
            return;
        }
    }
    present_mode.resize(1);
    present_mode[0] = VK_PRESENT_MODE_FIFO_KHR;
}

void RenderBackend::_QuerySwapchainSupport(
    VkSurfaceCapabilitiesKHR*        available_capabilities,
    std::vector<VkSurfaceFormatKHR>* available_formats,
    std::vector<VkPresentModeKHR>*   available_present_modes) {

    /* Capabilities */
    if (available_capabilities != nullptr) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_dev, _surface,
                                                  available_capabilities);
    }

    /* Supported Formats */
    if (available_formats != nullptr) {
        uint32 format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_dev, _surface, &format_count,
                                             nullptr);
        FFLY_ASSERT(format_count, "No Surface Formats supported by Device");
        available_formats->resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_dev, _surface, &format_count,
                                             available_formats->data());
    }

    /* Present Modes */
    if (available_present_modes != nullptr) {
        uint32 pres_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_dev, _surface,
                                                  &pres_mode_count, nullptr);
        FFLY_ASSERT(pres_mode_count, "No Surface Present Mode supported by Device");
        available_present_modes->resize(pres_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            _physical_dev, _surface, &pres_mode_count, available_present_modes->data());
    }
}
/* ===  ===  === === === === === === === ===*/

/* === === === === === === Commands === === === === === === */
void RenderBackend::_CreateCommandPoolAndBuffers() {
    uint32 graphics_family_index;
    _FindQueueFamilies(_physical_dev, &graphics_family_index, nullptr);

    VkCommandPoolCreateInfo create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    create_info.queueFamilyIndex        = graphics_family_index;

    VK_ASSERT(vkCreateCommandPool(_device, &create_info, nullptr, &_command_pool),
              "Failed to create CommandPool.");

    _render_command_buffers.resize(_swapchain_images.size());
}

void RenderBackend::_BeginCommandRecording(VkCommandBufferUsageFlags usage,
                                           VkCommandBuffer&          target) {
    VkCommandBufferAllocateInfo alloc_info = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool        = _command_pool;
    alloc_info.commandBufferCount = 1;

    VK_ASSERT(vkAllocateCommandBuffers(_device, &alloc_info, &target),
              "Failed to allocate CommandBuffer.");

    VkCommandBufferBeginInfo begin_info = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    begin_info.flags                    = usage;
    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    // VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT

    vkBeginCommandBuffer(target, &begin_info);
}

void RenderBackend::_EndCommandRecording(VkCommandBuffer& target) {
    vkEndCommandBuffer(target);
}

void RenderBackend::_EndCommandRecordingAndSubmit(VkCommandBuffer& target) {
    vkEndCommandBuffer(target);

    VkSubmitInfo submit_info       = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers    = &target;

    /* TODO: Fence */
    vkQueueSubmit(_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(_graphics_queue);

    vkFreeCommandBuffers(_device, _command_pool, 1, &target);
}

void RenderBackend::_CreateSyncObjects() {
    _acquired_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _released_semaphore.resize(MAX_FRAMES_IN_FLIGHT);
    _in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        SyncObjects::CreateSemaphore(_device, _acquired_semaphore[i]);
        SyncObjects::CreateSemaphore(_device, _released_semaphore[i]);
        SyncObjects::CreateFence(_device, _in_flight_fences[i]);
    }
}
/* ===  ===  === === === === === === === ===*/

/* === === === === === === PIPELINE === === === === === === */
void RenderBackend::_CreateRenderPass() {
    std::array<VkAttachmentDescription, 2> attachment_descrpt = {};
    /* Color */
    attachment_descrpt[0].format         = _swapchain_img_format;
    attachment_descrpt[0].samples        = VK_SAMPLE_COUNT_1_BIT;
    attachment_descrpt[0].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_descrpt[0].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    attachment_descrpt[0].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_descrpt[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descrpt[0].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_descrpt[0].finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    /* Depth */
    attachment_descrpt[1].format         = VK_FORMAT_D32_SFLOAT;
    attachment_descrpt[1].samples        = VK_SAMPLE_COUNT_1_BIT;
    attachment_descrpt[1].loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment_descrpt[1].storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descrpt[1].stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment_descrpt[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment_descrpt[1].initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment_descrpt[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    /* Attachment References */
    std::array<VkAttachmentReference, 2> attachment_refs;
    attachment_refs[0].attachment = 0;
    attachment_refs[0].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment_refs[1].attachment = 1;
    attachment_refs[1].layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    /* Subpass */
    VkSubpassDescription subpass    = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &attachment_refs[0];
    subpass.pDepthStencilAttachment = &attachment_refs[1];

    /* Synching Attachment Layout Transition between Subpasses */
    VkSubpassDependency dependency = {};
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass          = 0;
    dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask       = 0;
    dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    /* Renderpass */
    VkRenderPassCreateInfo renderpass_info = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    renderpass_info.attachmentCount = static_cast<uint32>(attachment_descrpt.size());
    renderpass_info.pAttachments    = attachment_descrpt.data();
    renderpass_info.subpassCount    = 1;
    renderpass_info.pSubpasses      = &subpass;
    renderpass_info.dependencyCount = 1;
    renderpass_info.pDependencies   = &dependency;

    VkRenderPass renderpass;
    VK_ASSERT(vkCreateRenderPass(_device, &renderpass_info, nullptr, &renderpass),
              "Failed to create RenderPass.");
    _renderpass.push_back(renderpass);
}

void RenderBackend::_RecordRenderPass() {
    VkClearColorValue           color        = {0.26f, 0.14f, 0.22f, 1.f};
    VkClearDepthStencilValue    depth        = {1, 0};
    std::array<VkClearValue, 2> clear_values = {};
    clear_values[0].color                    = color;
    clear_values[1].depthStencil             = depth;

    for (size_t i = 0; i < _swapchain_images.size(); i++) {
        _BeginCommandRecording(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                               _render_command_buffers[i]);

        VkRenderPassBeginInfo renderpass_info = {
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
        renderpass_info.renderPass        = _renderpass[0];
        renderpass_info.framebuffer       = _swapchain_framebuffers[i].Framebuffer;
        renderpass_info.renderArea.offset = {0, 0};
        renderpass_info.renderArea.extent = _swapchain_extent;
        renderpass_info.clearValueCount   = static_cast<uint32>(clear_values.size());
        renderpass_info.pClearValues      = clear_values.data();

        vkCmdBeginRenderPass(_render_command_buffers[i], &renderpass_info,
                             VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(_render_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          _graphics_pipeline);
        // vkCmdBindVertexBuffers();
        // vkCmdBindIndexBuffer();
        // vkCmdDrawIndexed();
        vkCmdDraw(_render_command_buffers[i], 3, 1, 0, 0);
        vkCmdEndRenderPass(_render_command_buffers[i]);
        _EndCommandRecording(_render_command_buffers[i]);
    }
}

void RenderBackend::_CreatePipeline() {
    Shader::Shader vert_shader, frag_shader;
    Shader::CreateShader(_device, "./Src/Rendering/Shaders/triangle.vert.spv",
                         vert_shader);
    Shader::CreateShader(_device, "./Src/Rendering/Shaders/triangle.frag.spv",
                         frag_shader);

    VkPipelineShaderStageCreateInfo shader_stages_info[2] = {};
    shader_stages_info[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages_info[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
    shader_stages_info[0].module = vert_shader.Module;
    shader_stages_info[0].pName  = "main";

    shader_stages_info[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stages_info[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_stages_info[1].module = frag_shader.Module;
    shader_stages_info[1].pName  = "main";

    auto binding_description = Shader::Vertex::GetBindingDescription();
    auto attrib_description  = Shader::Vertex::GetAttribDescription();

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    input_assembly_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    /* The Viewport has to be flipped because the Y Coordinate is reversed */
    VkViewport viewport = {};
    viewport.x          = 0.f;
    viewport.y          = (float)_swapchain_extent.height; /* Flip Viewport */
    viewport.width      = (float)_swapchain_extent.width;
    viewport.height     = -(float)_swapchain_extent.height; /* Flip Viewport */
    viewport.minDepth   = 0.f;
    viewport.maxDepth   = 1.f;

    VkRect2D scissor = {};
    scissor.offset   = {0, 0};
    scissor.extent   = _swapchain_extent;

    VkPipelineViewportStateCreateInfo viewport_info = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport_info.viewportCount = 1;
    viewport_info.pViewports    = &viewport;
    viewport_info.scissorCount  = 1;
    viewport_info.pScissors     = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer_info = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizer_info.depthClampEnable        = VK_FALSE;
    rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
    rasterizer_info.polygonMode             = VK_POLYGON_MODE_FILL;
    rasterizer_info.lineWidth               = 1.f;
    rasterizer_info.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizer_info.frontFace               = VK_FRONT_FACE_CLOCKWISE;
    rasterizer_info.depthBiasEnable         = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling_info = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampling_info.sampleShadingEnable  = VK_FALSE;
    multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depth_stencil_info.depthTestEnable       = VK_TRUE;
    depth_stencil_info.depthWriteEnable      = VK_TRUE;
    depth_stencil_info.depthCompareOp        = VK_COMPARE_OP_LESS;
    depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_info.minDepthBounds        = 0.f;
    depth_stencil_info.maxDepthBounds        = 1.f;
    depth_stencil_info.stencilTestEnable     = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorblend_attachment = {};
    colorblend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    colorblend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blending_info = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    color_blending_info.logicOpEnable   = VK_FALSE;
    color_blending_info.attachmentCount = 1;
    color_blending_info.pAttachments    = &colorblend_attachment;

    // VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT,
    //                                    VK_DYNAMIC_STATE_SCISSOR};

    // VkPipelineDynamicStateCreateInfo dynamic_state_info = {
    //     VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    // dynamic_state_info.dynamicStateCount =
    //     sizeof(dynamic_states) / sizeof(dynamic_states[0]);
    // dynamic_state_info.pDynamicStates = dynamic_states;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pipeline_layout_info.setLayoutCount         = 0;
    pipeline_layout_info.pushConstantRangeCount = 0;

    VK_ASSERT(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr,
                                     &_graphics_pipeline_layout),
              "Failed to create Graphics Pipeline Layout.");

    VkGraphicsPipelineCreateInfo create_info = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    create_info.stageCount          = 2;
    create_info.pStages             = shader_stages_info;
    create_info.pVertexInputState   = &vertex_input_info;
    create_info.pInputAssemblyState = &input_assembly_info;
    create_info.pViewportState      = &viewport_info;
    create_info.pRasterizationState = &rasterizer_info;
    create_info.pMultisampleState   = &multisampling_info;
    create_info.pDepthStencilState  = &depth_stencil_info;
    create_info.pColorBlendState    = &color_blending_info;
    create_info.pDynamicState       = nullptr;
    create_info.layout              = _graphics_pipeline_layout;
    create_info.renderPass          = _renderpass[0];
    create_info.subpass             = 0;

    VK_ASSERT(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &create_info,
                                        nullptr, &_graphics_pipeline),
              "Failed to create Graphics Pipeline.");

    vkDestroyShaderModule(_device, vert_shader.Module, nullptr);
    vkDestroyShaderModule(_device, frag_shader.Module, nullptr);
}

/* ===  ===  === === === === === === === ===*/

/* === === === === === === Presentation === === === === === === */
void RenderBackend::_CreatePresentationObjects() {
    uint32 img_count = _swapchain_images.size();
    _color_attachments.resize(img_count);
    _swapchain_framebuffers.resize(img_count);

    /* Depth Image */
    Presentation::CreateAttachment(
        _device, _swapchain_extent, nullptr, VK_FORMAT_D32_SFLOAT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, _depth_attachment);

    /* Color ImageViews and Framebuffers */
    for (uint32 i = 0; i < img_count; i++) {
        Presentation::CreateAttachment(
            _device, _swapchain_extent, &_swapchain_images[i], _swapchain_img_format,
            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, _color_attachments[i]);
        Presentation::CreateFramebuffer(_device, _renderpass[0], _swapchain_extent,
                                        _color_attachments[i], _depth_attachment,
                                        _swapchain_framebuffers[i]);
    }
}
/* ===  ===  === === === === === === === ===*/

/* ===  ===  === === === === === === === ===*/
/*
 *
 *
 */
/* === === === === === === CleanUp === === === === === === */
void RenderBackend::_CleanUp() {
    vkDeviceWaitIdle(_device);

    for (uint32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(_device, _acquired_semaphore[i], nullptr);
        vkDestroySemaphore(_device, _released_semaphore[i], nullptr);
        vkDestroyFence(_device, _in_flight_fences[i], nullptr);
    }

    vkDestroyPipeline(_device, _graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _graphics_pipeline_layout, nullptr);
    for (size_t i = 0; i < _renderpass.size(); i++) {
        vkDestroyRenderPass(_device, _renderpass[i], nullptr);
    }

    vkDestroyImageView(_device, _depth_attachment.View, nullptr);
    vkDestroyImage(_device, _depth_attachment.Image, nullptr);
    vkFreeMemory(_device, _depth_attachment.Memory, nullptr);
    vkDestroyFramebuffer(_device, _depth_framebuffer.Framebuffer, nullptr);
    for (size_t i = 0; i < _swapchain_images.size(); i++) {
        vkDestroyImageView(_device, _color_attachments[i].View, nullptr);
        vkDestroyFramebuffer(_device, _swapchain_framebuffers[i].Framebuffer, nullptr);
    }

    vkDestroyCommandPool(_device, _command_pool, nullptr);
    vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    vkDestroyDevice(_device, nullptr);
    DestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);
    vkDestroySurfaceKHR(_instance, _surface, nullptr);
    vkDestroyInstance(_instance, nullptr);
}
/* ===  ===  === === === === === === === === */

/* === === === === === === Debug Layer === === === === === === */
void RenderBackend::_SetupDebugLayer() {
    VkDebugUtilsMessengerCreateInfoEXT create_info = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = DebugCallback;
    create_info.pUserData       = nullptr;

    VK_ASSERT(CreateDebugUtilsMessengerEXT(_instance, &create_info, nullptr,
                                           &_debug_messenger),
              "Failed to Setup Vulkan Debug Messenger");
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
    VkDebugUtilsMessageTypeFlagsEXT             message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) {

    const char* message = p_callback_data->pMessage;
    if (message_severity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        FFLY_LOG_CORE_INFO("VULKAN::Validation Layer: {0}", message);
    } else {
        FFLY_LOG_CORE_ERROR("VULKAN::Validation Layer: {0}", message);
    }
    return VK_FALSE;
}
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(VkInstance                   instance,
                                   VkDebugUtilsMessengerEXT     messenger,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, messenger, pAllocator);
    }
}
/* ===  ===  === === === === === === */

} // namespace Rendering
} // namespace Firefly
