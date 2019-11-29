#pragma once

// #define VK_USE_PLATFORM_XCB_KHR
// #include <vulkan/vulkan.h>
// #include <vulkan/vulkan_xcb.h>
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX

// VkTools::Initializer::

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE /* Because vulkan has a depth range of 0.f           \
                                       to 1.f */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <gli/gli.hpp>
#include <stb/stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

const int WIDTH                = 1280;
const int HEIGHT               = 720;
const int MAX_FRAMES_IN_FLIGHT = 3;
// const int WIDTH  = 800;
// const int HEIGHT = 600;

const std::vector<const char*> glb_device_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

const std::vector<const char*> glb_validation_layers = {
    "VK_LAYER_LUNARG_standard_validation"};

const bool glb_enable_validation_layers = true;

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

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool IsComplete() {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities; /* Basic surface capabilities (min/max
                                              number of images in swap */
    /* chain, min/max width and height of images) */
    std::vector<VkSurfaceFormatKHR> formats; /* Surface formats (pixel format, */
                                             /* color space) */

    std::vector<VkPresentModeKHR> present_modes; /* Available presentations modes */
};

std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open shader file");
    }

    size_t            file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 tex_coord;

    static VkVertexInputBindingDescription GetBindingDescription() {
        VkVertexInputBindingDescription binding_description = {};
        binding_description.binding                         = 0;
        binding_description.stride                          = sizeof(Vertex);
        binding_description.inputRate                       = VK_VERTEX_INPUT_RATE_VERTEX;

        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttribDescription() {
        std::array<VkVertexInputAttributeDescription, 3> attrib_description = {};

        attrib_description[0].binding  = 0;
        attrib_description[0].location = 0;
        attrib_description[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_description[0].offset   = offsetof(Vertex, pos);

        attrib_description[1].binding  = 0;
        attrib_description[1].location = 1;
        attrib_description[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
        attrib_description[1].offset   = offsetof(Vertex, color);

        attrib_description[2].binding  = 0;
        attrib_description[2].location = 2;
        attrib_description[2].format   = VK_FORMAT_R32G32_SFLOAT;
        attrib_description[2].offset   = offsetof(Vertex, tex_coord);

        return attrib_description;
    }
};

const std::vector<Vertex> cube_vertices = {
    /* Front */
    {{-1.0, -1.0, 1.0}, {1.f, 0.f, 0.f}},
    {{1.0, -1.0, 1.0}, {1.f, 0.f, 0.f}},
    {{1.0, 1.0, 1.0}, {0.f, 1.f, 0.f}},
    {{-1.0, 1.0, 1.0}, {0.f, 1.f, 0.f}},
    /* Back */
    {{-1.0, -1.0, -1.0}, {0.f, 0.f, 1.f}},
    {{1.0, -1.0, -1.0}, {0.f, 0.f, 1.f}},
    {{1.0, 1.0, -1.0}, {0.f, 1.f, 1.f}},
    {{-1.0, 1.0, -1.0}, {0.f, 1.f, 1.f}}};

const std::vector<uint16_t> cube_indices = {
    // front
    0, 1, 2, 2, 3, 0,
    // right
    1, 5, 6, 6, 2, 1,
    // back
    7, 6, 5, 5, 4, 7,
    // left
    4, 0, 3, 3, 7, 4,
    // bottom
    4, 5, 1, 1, 0, 4,
    // top
    3, 2, 6, 6, 7, 3};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

//{0.26f, 0.23f, 0.31f, 1.0f}

class Application {
    friend class DeferredRenderer;
  public:
    void Run() {
        _InitWindow();
        _InitVulkan();
        _MainLoop();
        _Cleanup();
    }

  private:
    void _InitWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void _MainLoop() {
        double   last_frame_time = glfwGetTime();
        uint32_t nb_frames       = 0;

        while (!glfwWindowShouldClose(_window)) {
            double curr_frame_time = glfwGetTime();
            nb_frames++;
            /* If there is 1sec since last cout */
            if (curr_frame_time - last_frame_time >= 1.0) {
                std::cout << nb_frames << "fps" << std::endl;
                std::cout << 1000.0 / double(nb_frames) << "ms" << std::endl;
                nb_frames = 0;
                last_frame_time += 1.0;
            }
            glfwPollEvents();
            _DrawFrame();
        }
        vkDeviceWaitIdle(_device);
    }

    void _Cleanup() {
        vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
        vkDestroyDescriptorSetLayout(_device, _descriptor_set_layout, nullptr);

        vkDestroyImageView(_device, _depth_img_view, nullptr);
        vkDestroyImage(_device, _depth_image, nullptr);
        vkFreeMemory(_device, _depth_img_memory, nullptr);
        vkDestroySampler(_device, _cubemap_sampler, nullptr);
        vkDestroyImageView(_device, _cubemap_img_view, nullptr);
        vkDestroyImage(_device, _cubemap_image, nullptr);
        vkFreeMemory(_device, _cubemap_img_memory, nullptr);
        vkDestroySampler(_device, _texture_sampler, nullptr);
        vkDestroyImageView(_device, _texture_img_view, nullptr);
        vkDestroyImage(_device, _texture_image, nullptr);
        vkFreeMemory(_device, _texture_img_memory, nullptr);

        for (size_t i = 0; i < _swapchain_images.size(); i++) {
            vkDestroyBuffer(_device, _uniform_buffers[i], nullptr);
            vkFreeMemory(_device, _uniform_buffers_memory[i], nullptr);
            vkDestroyBuffer(_device, _uniform_buffers_cubemap[i], nullptr);
            vkFreeMemory(_device, _uniform_buffers_cubemap_memory[i], nullptr);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(_device, _semaphores_render_finished[i], nullptr);
            vkDestroySemaphore(_device, _semaphores_img_available[i], nullptr);
            vkDestroyFence(_device, _fences_inflight[i], nullptr);
        }
        vkDestroyCommandPool(_device, _command_pool, nullptr);
        for (auto framebuffer : _swapchain_framebuffers) {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
        }

        vkDestroyBuffer(_device, _index_buffer, nullptr);
        vkFreeMemory(_device, _index_buffer_memory, nullptr);
        vkDestroyBuffer(_device, _vertex_buffer, nullptr);
        vkFreeMemory(_device, _vertex_buffer_memory, nullptr);

        vkDestroyPipeline(_device, _graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
        vkDestroyRenderPass(_device, _renderpass, nullptr);
        for (auto img_view : _swapchain_img_views) {
            vkDestroyImageView(_device, img_view, nullptr);
        }
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);

        vkDestroyDevice(_device, nullptr);
        if (glb_enable_validation_layers) {
            DestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);
        }
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void _InitVulkan() {
        _CreateInstance();
        _SetupDebugMessenger();
        _CreateSurface();
        _PickPhysicalDevice();
        _CreateLogicalDevice();
        _CreateSwapChain();
        _CreateImageViews();
        _CreateCommandPool();
        _CreateDepthResources();
        _CreateRenderpPass();
        _CreateDescriptorSetLayout();
        _CreateGraphisPipeline();
        _CreateFrameBuffers();
        _texture_image =
            _CreateTextureImage("./Textures/chalet.jpg", _texture_img_memory, 0);
        _cubemap_image =
            _CreateTextureImage("./Textures/cubemap_space.ktx", _cubemap_img_memory, 1);
        _texture_img_view = _CreateTextureImageView(_texture_image);
        _cubemap_img_view =
            _CreateTextureImageView(_cubemap_image, VK_FORMAT_BC3_UNORM_BLOCK);
        _texture_sampler = _CreateTextureSampler();
        _cubemap_sampler = _CreateTextureSampler();
        _LoadModel();
        _CreateIndexBuffer();
        _CreateVertexBuffer();

        _CreateUniformBuffers();
        _CreateDescriptorPool();
        _CreateDescriptorSets();

        _CreateCommandBuffers();
        _CreateSyncObjects();
    }

    void _CreateInstance() {
        if (glb_enable_validation_layers && !_CheckValidationLayerSupport()) {
            throw std::runtime_error("Validation layers requested, but not available");
        }

        VkApplicationInfo app_info  = {};
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = "VulkanTriangle";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = "Firefly";
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = VK_API_VERSION_1_0;

        /* To get a list of all supported extensions */
        uint32_t ext_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);
        std::vector<VkExtensionProperties> available_extensions(ext_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &ext_count,
                                               available_extensions.data());

        auto extensions = _GetRequiredExtensions();
        for (auto ext : extensions) {
            std::cout << ext << std::endl;
        }
        std::cout << "====================" << std::endl;

        VkInstanceCreateInfo create_info    = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo        = &app_info;
        create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        if (glb_enable_validation_layers) {
            create_info.enabledLayerCount =
                static_cast<uint32_t>(glb_validation_layers.size());
            create_info.ppEnabledLayerNames = glb_validation_layers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan Instance");
        }
    }
    void _CreateSurface() {
        // VkXcbSurfaceCreateInfoKHR xcb_create_info = {};
        // xcb_create_info.sType  = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        // xcb_create_info.window = glfwGetX11Window(_window);
        // if(vkCreateXcbSurfaceKHR(_instance,&xcb_create_info,nullptr,&_surface)!=VK_SUCCESS){
        //     throw std::runtime_error("Failed to create window surface");
        // }
        if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }
    std::vector<const char*> _GetRequiredExtensions() {
        uint32_t     glfw_ext_count = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

        std::vector<const char*> extensions(glfw_extensions,
                                            glfw_extensions + glfw_ext_count);
        if (glb_enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool _CheckValidationLayerSupport() {
        uint32_t layer_count = 0;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

        std::vector<VkLayerProperties> available_layers(layer_count);
        vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

        for (const char* layer_name : glb_validation_layers) {
            bool layer_found = false;

            for (const auto& layer_properties : available_layers) {
                if (strcmp(layer_name, layer_properties.layerName) == 0) {
                    layer_found = true;
                    break;
                }
            }
            if (!layer_found) {
                return false;
            }
        }

        return true;
    }

    void _SetupDebugMessenger() {
        if (!glb_enable_validation_layers)
            return;
        VkDebugUtilsMessengerCreateInfoEXT create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        create_info.pfnUserCallback = _DebugCallback;
        create_info.pUserData       = nullptr;

        if (CreateDebugUtilsMessengerEXT(_instance, &create_info, nullptr,
                                         &_debug_messenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger");
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL _DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      message_severity,
        VkDebugUtilsMessageTypeFlagsEXT             message_type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data) {

        const char* message = p_callback_data->pMessage;
        std::cerr << "Validation Layer:" << message << std::endl;
        return VK_FALSE;
    }

    void _PickPhysicalDevice() {
        uint32_t dev_count = 0;
        vkEnumeratePhysicalDevices(_instance, &dev_count, nullptr);
        if (dev_count == 0) {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }
        std::cout << "DevCount:" << dev_count << std::endl;
        std::vector<VkPhysicalDevice> devices(dev_count);
        vkEnumeratePhysicalDevices(_instance, &dev_count, devices.data());

        for (const auto& device : devices) {
            if (_IsDeviceSuitable(device)) {
                _physical_dev = device;
                break;
            }
        }

        if (_physical_dev == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU");
        }

        VkPhysicalDeviceProperties dev_properties = {};
        vkGetPhysicalDeviceProperties(_physical_dev, &dev_properties);
        std::cout << "Device used:" << dev_properties.deviceName << std::endl
                  << "API_VERSION:" << dev_properties.apiVersion << std::endl;
    }

    bool _IsDeviceSuitable(VkPhysicalDevice dev) {
        VkPhysicalDeviceProperties dev_properties;
        vkGetPhysicalDeviceProperties(dev, &dev_properties);

        VkPhysicalDeviceFeatures dev_features;
        vkGetPhysicalDeviceFeatures(dev, &dev_features);

        QueueFamilyIndices indices              = _FindQueueFamilies(dev);
        bool               extensions_supported = _CheckDeviceExtensionSupport(dev);
        bool               swapchain_adequate   = false;
        if (extensions_supported) {
            SwapChainSupportDetails swapchain_support = _QuerySwapChainSupport(dev);
            swapchain_adequate = !swapchain_support.formats.empty() &&
                                 !swapchain_support.present_modes.empty();
        }
        return indices.IsComplete() && extensions_supported && swapchain_adequate &&
               dev_features.samplerAnisotropy;
    }

    bool _CheckDeviceExtensionSupport(VkPhysicalDevice dev) {
        uint32_t ext_count;
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &ext_count, nullptr);
        std::vector<VkExtensionProperties> dev_available_ext(ext_count);
        vkEnumerateDeviceExtensionProperties(dev, nullptr, &ext_count,
                                             dev_available_ext.data());

        std::set<std::string> required_ext(glb_device_extensions.begin(),
                                           glb_device_extensions.end());
        for (const auto& ext : dev_available_ext) {
            required_ext.erase(ext.extensionName);
        }

        return required_ext.empty();
    }

    void _CreateLogicalDevice() {
        QueueFamilyIndices indices               = _FindQueueFamilies(_physical_dev);
        std::set<uint32_t> queue_family_indinces = {indices.graphics_family.value(),
                                                    indices.present_family.value()};

        float queue_priority = 1.0f;

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        for (uint32_t queue_familiy_index : queue_family_indinces) {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_familiy_index;
            queue_create_info.queueCount       = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures dev_features = {};
        dev_features.samplerAnisotropy        = VK_TRUE;

        VkDeviceCreateInfo create_info = {};
        create_info.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount =
            static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.pEnabledFeatures  = &dev_features;
        create_info.enabledExtensionCount =
            static_cast<uint32_t>(glb_device_extensions.size());
        create_info.ppEnabledExtensionNames = glb_device_extensions.data();
        if (glb_enable_validation_layers) {
            create_info.enabledLayerCount =
                static_cast<uint32_t>(glb_validation_layers.size());
            create_info.ppEnabledLayerNames = glb_validation_layers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(_physical_dev, &create_info, nullptr, &_device) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device");
        }

        vkGetDeviceQueue(_device, indices.graphics_family.value(), 0, &_graphics_queue);
        vkGetDeviceQueue(_device, indices.present_family.value(), 0, &_present_queue);
    }

    QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice dev) {
        QueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queue_family_count,
                                                 queue_families.data());

        uint32_t i = 0;
        for (const auto& queue_family : queue_families) {
            if (queue_family.queueCount > 0 &&
                queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphics_family = i;
            }

            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, _surface, &present_support);
            if (queue_family.queueCount > 0 && present_support) {
                indices.present_family = i;
            }

            if (indices.IsComplete()) {
                break;
            }
            i++;
        }

        return indices;
    }

    SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice dev) {
        SwapChainSupportDetails details;

        /* Capabilities */
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(dev, _surface, &details.capabilities);

        /* Supported Formats */
        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(dev, _surface, &format_count, nullptr);
        if (format_count > 0) {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(dev, _surface, &format_count,
                                                 details.formats.data());
        }

        /* Present Modes */
        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(dev, _surface, &present_mode_count,
                                                  nullptr);
        if (present_mode_count > 0) {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(dev, _surface, &present_mode_count,
                                                      details.present_modes.data());
        }

        return details;
    }

    void _CreateSwapChain() {
        SwapChainSupportDetails swapchain_support = _QuerySwapChainSupport(_physical_dev);

        /* Color depth (RGB, SRGB, etc) */
        VkSurfaceFormatKHR surface_format =
            _ChooseSwapSurfaceFormat(swapchain_support.formats);
        /* Conditions for "swapping" images to screen (vsync, etc) */
        VkPresentModeKHR present_mode =
            _ChooseSwapPresentModel(swapchain_support.present_modes);
        std::cout << "PresentMode:" << present_mode << std::endl;
        /* Resolution of images in swapchain */
        VkExtent2D extent = _ChooseSwapExtent(swapchain_support.capabilities);

        uint32_t image_count = 3; /* Double Buffering */
        /* If maxImageCount ==0, there is no maximum */
        if (swapchain_support.capabilities.maxImageCount > 0 &&
            image_count > swapchain_support.capabilities.maxImageCount) {
            image_count = swapchain_support.capabilities.maxImageCount;
        }
        std::cout << "ImageCount:" << image_count << std::endl;

        VkSwapchainCreateInfoKHR create_info = {};
        create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface          = _surface;
        create_info.minImageCount    = image_count;
        create_info.imageFormat      = surface_format.format;
        create_info.imageColorSpace  = surface_format.colorSpace;
        create_info.imageExtent      = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices                 = _FindQueueFamilies(_physical_dev);
        uint32_t           queue_familiy_indices[] = {indices.graphics_family.value(),
                                            indices.present_family.value()};

        if (indices.graphics_family != indices.present_family) {
            create_info.imageSharingMode =
                VK_SHARING_MODE_CONCURRENT; /* Allow Image Sharing between Queues */
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = queue_familiy_indices;
        } else {
            create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;       /* Optional */
            create_info.pQueueFamilyIndices   = nullptr; /* Optional */
        }

        /* To specify that no transforms (clockwise rotation, etc) is applied to
         * images in swapchain */
        create_info.preTransform   = swapchain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode    = present_mode;
        create_info.clipped        = VK_TRUE;
        create_info.oldSwapchain   = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(_device, &create_info, nullptr, &_swapchain) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create Swapchain");
        }

        vkGetSwapchainImagesKHR(_device, _swapchain, &image_count, nullptr);
        _swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(_device, _swapchain, &image_count,
                                _swapchain_images.data());

        _swapchain_img_format = surface_format.format;
        _swapchain_extent     = extent;
    }

    VkSurfaceFormatKHR
    _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats) {
        if (available_formats.size() == 1 &&
            available_formats[0].format == VK_FORMAT_UNDEFINED) {
            return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }

        for (const auto& available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }

        return available_formats[0];
    }

    VkPresentModeKHR
    _ChooseSwapPresentModel(const std::vector<VkPresentModeKHR> available_present_modes) {
        for (const auto& available_present_mode : available_present_modes) {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return VK_PRESENT_MODE_MAILBOX_KHR;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        /* If currentExtent.width = max of uint32_t, the resolution of the swap
         * chain has no limit */
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actual_extent = {WIDTH, HEIGHT};

            actual_extent.width = std::max(
                capabilities.minImageExtent.width,
                std::min(capabilities.maxImageExtent.width, actual_extent.width));
            actual_extent.height = std::max(
                capabilities.minImageExtent.height,
                std::min(capabilities.maxImageExtent.height, actual_extent.height));

            return actual_extent;
        }
    }

    void _CreateImageViews() {
        _swapchain_img_views.resize(_swapchain_images.size());
        for (size_t i = 0; i < _swapchain_images.size(); i++) {
            _swapchain_img_views[i] = _CreateImageView(
                _swapchain_images[i], _swapchain_img_format, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void _CreateRenderpPass() {
        VkAttachmentDescription color_attachment = {};
        color_attachment.format                  = _swapchain_img_format;
        color_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment            = 0; /* Index of the attachment. */
        color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment = {};
        depth_attachment.format                  = VK_FORMAT_D32_SFLOAT;
        depth_attachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref = {};
        depth_attachment_ref.attachment            = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass    = {};
        subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount    = 1;
        subpass.pColorAttachments       = &color_attachment_ref;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        std::array<VkAttachmentDescription, 2> attachments = {color_attachment,
                                                              depth_attachment};

        VkSubpassDependency dependency = {};
        dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass          = 0;
        dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask       = 0;
        dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderpass_info = {};
        renderpass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderpass_info.pAttachments    = attachments.data();
        renderpass_info.subpassCount    = 1;
        renderpass_info.pSubpasses      = &subpass;
        renderpass_info.dependencyCount = 1;
        renderpass_info.pDependencies   = &dependency;

        if (vkCreateRenderPass(_device, &renderpass_info, nullptr, &_renderpass) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create RenderPass");
        }
    }

    void _CreateGraphisPipeline() {
        auto bl           = ReadFile("./Shaders/shader.vert");
        auto vert_shdcode = ReadFile("./Shaders/vert.spv");
        auto frag_shdcode = ReadFile("./Shaders/frag.spv");

        VkShaderModule vertex_module   = _CreateShaderModule(vert_shdcode);
        VkShaderModule fragment_module = _CreateShaderModule(frag_shdcode);

        std::cout << vert_shdcode.size() << std::endl << frag_shdcode.size() << std::endl;

        VkPipelineShaderStageCreateInfo vertex_stage_info = {};
        vertex_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertex_stage_info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertex_stage_info.module = vertex_module;
        vertex_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo fragment_stage_info = {};
        fragment_stage_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragment_stage_info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragment_stage_info.module = fragment_module;
        fragment_stage_info.pName  = "main";

        VkPipelineShaderStageCreateInfo shader_stages_info[] = {vertex_stage_info,
                                                                fragment_stage_info};

        auto binding_description = Vertex::GetBindingDescription();
        auto attrib_description  = Vertex::GetAttribDescription();

        VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
        vertex_input_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.pVertexBindingDescriptions    = &binding_description;
        vertex_input_info.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attrib_description.size());
        vertex_input_info.pVertexAttributeDescriptions = attrib_description.data();

        VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
        input_assembly_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        input_assembly_info.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        input_assembly_info.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport = {};
        viewport.x          = 0.f;
        viewport.y          = 0.f;
        viewport.width      = (float)_swapchain_extent.width;
        viewport.height     = (float)_swapchain_extent.height;
        viewport.minDepth   = 0.f;
        viewport.maxDepth   = 1.f;

        VkRect2D scissor = {};
        scissor.offset   = {0, 0};
        scissor.extent   = _swapchain_extent;

        VkPipelineViewportStateCreateInfo viewport_state_info = {};
        viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state_info.viewportCount = 1;
        viewport_state_info.pViewports    = &viewport;
        viewport_state_info.scissorCount  = 1;
        viewport_state_info.pScissors     = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer_info = {};
        rasterizer_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer_info.depthClampEnable        = VK_FALSE;
        rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
        rasterizer_info.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer_info.lineWidth               = 1.f;
        rasterizer_info.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer_info.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer_info.depthBiasEnable         = VK_FALSE;
        rasterizer_info.depthBiasConstantFactor = 0.f; /* Optional */
        rasterizer_info.depthBiasClamp          = 0.f; /* Optional */
        rasterizer_info.depthBiasSlopeFactor    = 0.f; /* Optional */

        VkPipelineMultisampleStateCreateInfo multisampling_info = {};
        multisampling_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling_info.sampleShadingEnable   = VK_FALSE;
        multisampling_info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampling_info.minSampleShading      = 1.0f;     /* Optional */
        multisampling_info.pSampleMask           = nullptr;  /* Optional */
        multisampling_info.alphaToCoverageEnable = VK_FALSE; /* Optional */
        multisampling_info.alphaToOneEnable      = VK_FALSE; /* Optional */

        VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
        depth_stencil_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_info.depthTestEnable       = VK_TRUE;
        depth_stencil_info.depthWriteEnable      = VK_TRUE;
        depth_stencil_info.depthCompareOp        = VK_COMPARE_OP_LESS;
        depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
        depth_stencil_info.minDepthBounds        = 0.f;
        depth_stencil_info.maxDepthBounds        = 1.f;
        depth_stencil_info.stencilTestEnable     = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorblend_attachment = {};
        colorblend_attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorblend_attachment.blendEnable         = VK_FALSE;
        colorblend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  /* Optional */
        colorblend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; /* Optional */
        colorblend_attachment.colorBlendOp        = VK_BLEND_OP_ADD;      /* Optional */
        colorblend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  /* Optional */
        colorblend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; /* Optional */
        colorblend_attachment.alphaBlendOp        = VK_BLEND_OP_ADD;      /* Optional */

        VkPipelineColorBlendStateCreateInfo color_blending_info = {};
        color_blending_info.sType =
            VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending_info.logicOpEnable     = VK_FALSE;
        color_blending_info.logicOp           = VK_LOGIC_OP_COPY; /* Optional */
        color_blending_info.attachmentCount   = 1;
        color_blending_info.pAttachments      = &colorblend_attachment;
        color_blending_info.blendConstants[0] = 0.0f; /* Optional */
        color_blending_info.blendConstants[1] = 0.0f; /* Optional */
        color_blending_info.blendConstants[2] = 0.0f; /* Optional */
        color_blending_info.blendConstants[3] = 0.0f; /* Optional */

        VkPipelineLayoutCreateInfo pipeline_layout_info = {};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount         = 1;
        pipeline_layout_info.pSetLayouts            = &_descriptor_set_layout;
        pipeline_layout_info.pushConstantRangeCount = 0;
        pipeline_layout_info.pPushConstantRanges    = nullptr;

        if (vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr,
                                   &_pipeline_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.sType             = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount        = 2;
        pipeline_info.pStages           = shader_stages_info;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &input_assembly_info;
        pipeline_info.pViewportState      = &viewport_state_info;
        pipeline_info.pRasterizationState = &rasterizer_info;
        pipeline_info.pMultisampleState   = &multisampling_info;
        pipeline_info.pDepthStencilState  = &depth_stencil_info;
        pipeline_info.pColorBlendState    = &color_blending_info;
        pipeline_info.pDynamicState       = nullptr;
        pipeline_info.layout              = _pipeline_layout;
        pipeline_info.renderPass          = _renderpass;
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE; /* Optional */
        pipeline_info.basePipelineIndex   = -1;             /* Optional */

        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr,
                                      &_graphics_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create GraphicsPipeline");
        }

        vkDestroyShaderModule(_device, vertex_module, nullptr);
        vkDestroyShaderModule(_device, fragment_module, nullptr);
    }

    VkShaderModule _CreateShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(_device, &create_info, nullptr, &shader_module) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }
        return shader_module;
    }

    void _CreateFrameBuffers() {
        _swapchain_framebuffers.resize(_swapchain_img_views.size());

        for (size_t i = 0; i < _swapchain_img_views.size(); i++) {
            std::array<VkImageView, 2> attachments = {_swapchain_img_views[i],
                                                      _depth_img_view};

            VkFramebufferCreateInfo framebuffer_info = {};
            framebuffer_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass      = _renderpass;
            framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_info.pAttachments    = attachments.data();
            framebuffer_info.width           = _swapchain_extent.width;
            framebuffer_info.height          = _swapchain_extent.height;
            framebuffer_info.layers          = 1;

            if (vkCreateFramebuffer(_device, &framebuffer_info, nullptr,
                                    &_swapchain_framebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create Framebuffer");
            }
        }
    }

    void _CreateCommandPool() {
        QueueFamilyIndices      qufamily_indices = _FindQueueFamilies(_physical_dev);
        VkCommandPoolCreateInfo pool_info        = {};
        pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.queueFamilyIndex = qufamily_indices.graphics_family.value();
        pool_info.flags            = 0; /* Optional */

        if (vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create CommandPool");
        }
    }

    void _CreateCommandBuffers() {
        _command_buffers.resize(_swapchain_framebuffers.size());
        for (size_t i = 0; i < _swapchain_framebuffers.size(); i++) {
            VkCommandBufferAllocateInfo alloc_info = {};
            alloc_info.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.commandPool = _command_pool;
            alloc_info.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandBufferCount = (uint32_t)_command_buffers.size();

            if (vkAllocateCommandBuffers(_device, &alloc_info, _command_buffers.data()) !=
                VK_SUCCESS) {
                throw std::runtime_error("Failed to allocate CommandBuffers");
            }
        }

        for (size_t i = 0; i < _command_buffers.size(); i++) {
            VkCommandBufferBeginInfo begin_info = {};
            begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            begin_info.pInheritanceInfo = nullptr; /* Optional */

            if (vkBeginCommandBuffer(_command_buffers[i], &begin_info) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording CommandBuffer");
            }

            VkRenderPassBeginInfo renderpass_info = {};
            renderpass_info.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpass_info.renderPass        = _renderpass;
            renderpass_info.framebuffer       = _swapchain_framebuffers[i];
            renderpass_info.renderArea.offset = {0, 0};
            renderpass_info.renderArea.extent = _swapchain_extent;
            std::array<VkClearValue, 2> clear_values = {};
            clear_values[0].color                    = {0.26f, 0.23f, 0.31f, 1.0f};
            clear_values[1].depthStencil             = {1, 0};
            renderpass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
            renderpass_info.pClearValues    = clear_values.data();

            vkCmdBeginRenderPass(_command_buffers[i], &renderpass_info,
                                 VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              _graphics_pipeline);

            VkBuffer     vertex_buffers[] = {_vertex_buffer};
            VkDeviceSize offsets[]        = {0};
            vkCmdBindVertexBuffers(_command_buffers[i], 0, 1, vertex_buffers, offsets);
            vkCmdBindIndexBuffer(_command_buffers[i], _index_buffer, 0,
                                 VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    _pipeline_layout, 0, 1, &_descriptor_sets[i], 0,
                                    nullptr);
            vkCmdDrawIndexed(_command_buffers[i], static_cast<uint32_t>(_indices.size()),
                             1, 0, 0, 0);

            vkCmdEndRenderPass(_command_buffers[i]);
            if (vkEndCommandBuffer(_command_buffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record CommandBuffer");
            }
        }
    }

    void _DrawFrame() {
        vkWaitForFences(_device, 1, &_fences_inflight[_current_frame], VK_TRUE,
                        std::numeric_limits<uint64_t>::max());
        vkResetFences(_device, 1, &_fences_inflight[_current_frame]);

        uint32_t img_index;
        vkAcquireNextImageKHR(_device, _swapchain, std::numeric_limits<uint64_t>::max(),
                              _semaphores_img_available[_current_frame], VK_NULL_HANDLE,
                              &img_index);

        VkSemaphore wait_semaphores[]      = {_semaphores_img_available[_current_frame]};
        VkPipelineStageFlags wait_stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        VkSemaphore signal_semaphores[] = {_semaphores_render_finished[_current_frame]};

        _UpdateUniformBuffers(img_index);

        VkSubmitInfo submit_info         = {};
        submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount   = 1;
        submit_info.pWaitSemaphores      = wait_semaphores;
        submit_info.pWaitDstStageMask    = wait_stages;
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &_command_buffers[img_index];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = signal_semaphores;

        if (vkQueueSubmit(_graphics_queue, 1, &submit_info,
                          _fences_inflight[_current_frame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit DrawCommandBuffer");
        }

        VkPresentInfoKHR present_info   = {};
        present_info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = signal_semaphores;

        VkSwapchainKHR swapchains[] = {_swapchain};
        present_info.swapchainCount = 1;
        present_info.pSwapchains    = swapchains;
        present_info.pImageIndices  = &img_index;
        present_info.pResults       = nullptr; /* Optional */

        vkQueuePresentKHR(_present_queue, &present_info);

        _current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void _CreateSyncObjects() {
        _semaphores_img_available.resize(MAX_FRAMES_IN_FLIGHT);
        _semaphores_render_finished.resize(MAX_FRAMES_IN_FLIGHT);
        _fences_inflight.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphore_info = {};
        semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info = {};
        fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(_device, &semaphore_info, nullptr,
                                  &_semaphores_img_available[i]) != VK_SUCCESS ||
                vkCreateSemaphore(_device, &semaphore_info, nullptr,
                                  &_semaphores_render_finished[i]) != VK_SUCCESS) {

                throw std::runtime_error("Failed to create Semaphores");
            }
            if (vkCreateFence(_device, &fence_info, nullptr, &_fences_inflight[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("Failed to create Fences");
            }
        }
    }

    void _LoadModel() {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;
        std::string                      warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                              "./Models/chalet.obj")) {
            throw std::runtime_error(warn + err);
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};
                vertex.pos    = {attrib.vertices[3 * index.vertex_index + 0],
                              attrib.vertices[3 * index.vertex_index + 1],
                              attrib.vertices[3 * index.vertex_index + 2]};

                vertex.color = {1.f, 1.f, 1.f};

                vertex.tex_coord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                    1.0 - attrib.texcoords[2 * index.texcoord_index +
                                                           1]}; /* Inverse Y coord */
                _vertices.push_back(vertex);
                _indices.push_back(_indices.size());
            }
        }
    }

    void _CreateVertexBuffer() {
        VkDeviceSize buffer_size = sizeof(_vertices[0]) * _vertices.size();

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        _CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, _vertices.data(), (size_t)buffer_size);
        vkUnmapMemory(_device, staging_buffer_memory);

        _CreateBuffer(
            buffer_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertex_buffer, _vertex_buffer_memory);

        _CopyBuffer(staging_buffer, _vertex_buffer, buffer_size);

        vkDestroyBuffer(_device, staging_buffer, nullptr);
        vkFreeMemory(_device, staging_buffer_memory, nullptr);
    }

    void _CreateIndexBuffer() {
        VkDeviceSize buffer_size = sizeof(_indices[0]) * _indices.size();

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_memory;
        _CreateBuffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                      staging_buffer, staging_buffer_memory);

        void* data;
        vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
        memcpy(data, _indices.data(), (size_t)buffer_size);
        vkUnmapMemory(_device, staging_buffer_memory);

        _CreateBuffer(buffer_size,
                      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _index_buffer,
                      _index_buffer_memory);
        _CopyBuffer(staging_buffer, _index_buffer, buffer_size);

        vkDestroyBuffer(_device, staging_buffer, nullptr);
        vkFreeMemory(_device, staging_buffer_memory, nullptr);
    }

    void _CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                       VkMemoryPropertyFlags properties, VkBuffer& buffer,
                       VkDeviceMemory& buffer_mem) {

        VkBufferCreateInfo buffer_info = {};
        buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size               = size;
        buffer_info.usage              = usage;
        buffer_info.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Buffer");
        }

        VkMemoryRequirements mem_requirement;
        vkGetBufferMemoryRequirements(_device, buffer, &mem_requirement);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize       = mem_requirement.size;
        alloc_info.memoryTypeIndex =
            _FindMemoryType(mem_requirement.memoryTypeBits, properties);

        if (vkAllocateMemory(_device, &alloc_info, nullptr, &buffer_mem) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate Buffer memory");
        }

        vkBindBufferMemory(_device, buffer, buffer_mem, 0);
    }

    void _CopyBuffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
        VkCommandBuffer command_buffer = _BeginSingleTimeCommands();

        VkBufferCopy copy_region = {};
        copy_region.srcOffset    = 0; /* Optional */
        copy_region.dstOffset    = 0; /* Optional */
        copy_region.size         = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        _EndSingleTimeCommands(command_buffer);
    }

    void _CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                            uint32_t height, uint32_t face_count = 1,
                            VkDeviceSize img_size = 0) {
        VkCommandBuffer command_buffer = _BeginSingleTimeCommands();

        std::vector<VkBufferImageCopy> cpy_regions;
        uint32_t                       offset = 0;
        for (uint32_t face = 0; face < face_count; face++) {

            VkBufferImageCopy cpy_region = {};
            cpy_region.bufferOffset      = offset; /* Use for cubemap */
            cpy_region.bufferRowLength   = 0;
            cpy_region.bufferImageHeight = 0;

            cpy_region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            cpy_region.imageSubresource.mipLevel       = 0;
            cpy_region.imageSubresource.baseArrayLayer = face; /* Use for cubemap */
            cpy_region.imageSubresource.layerCount     = 1;

            cpy_region.imageOffset = {0, 0, 0};
            cpy_region.imageExtent = {width, height, 1};

            cpy_regions.push_back(cpy_region);

            offset += img_size;
        }
        vkCmdCopyBufferToImage(
            command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            static_cast<uint32_t>(cpy_regions.size()), cpy_regions.data());

        _EndSingleTimeCommands(command_buffer);
    }

    uint32_t _FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(_physical_dev, &mem_properties);

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if (type_filter & (1 << i) && ((mem_properties.memoryTypes[i].propertyFlags &
                                            properties) == properties)) {
                return i;
            }
        }

        throw std::runtime_error("Failed to find memory type");
    }

    void _CreateUniformBuffers() {
        VkDeviceSize buffer_size = sizeof(UniformBufferObject);

        _uniform_buffers.resize(_swapchain_images.size());
        _uniform_buffers_memory.resize(_swapchain_images.size());
        _uniform_buffers_cubemap.resize(_swapchain_images.size());
        _uniform_buffers_cubemap_memory.resize(_swapchain_images.size());

        for (size_t i = 0; i < _swapchain_images.size(); i++) {
            _CreateBuffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                              VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                          _uniform_buffers[i], _uniform_buffers_memory[i]);

            _CreateBuffer(
                buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                _uniform_buffers_cubemap[i], _uniform_buffers_cubemap_memory[i]);
        }
    }

    void _UpdateUniformBuffers(uint32_t current_img) {
        static auto start_time   = glfwGetTime();
        auto        current_time = glfwGetTime();
        float       dtime        = current_time - start_time;

        UniformBufferObject ubo = {};

        ubo.model = glm::rotate(glm::mat4(1.f), dtime * glm::radians(45.f),
                                glm::vec3(0.f, 0.f, 1.f));

        // ubo.model =
        //     glm::rotate(glm::mat4(1.f), glm::radians(230.f), glm::vec3(0.f, 0.f, 1.f));
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.5f, 2.2f), glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(
            glm::radians(45.0f),
            _swapchain_extent.width / (float)_swapchain_extent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        void* data;
        vkMapMemory(_device, _uniform_buffers_memory[current_img], 0, sizeof(ubo), 0,
                    &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(_device, _uniform_buffers_memory[current_img]);

        /* CUBEMAP */
        UniformBufferObject cubemap = {};
    }

    void _CreateDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding ubo_layout_binding = {};
        ubo_layout_binding.binding                      = 0;
        ubo_layout_binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        ubo_layout_binding.descriptorCount = 1;
        ubo_layout_binding.stageFlags      = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutBinding sampler_layout_binding = {};
        sampler_layout_binding.binding                      = 1;
        sampler_layout_binding.descriptorCount              = 1;
        sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sampler_layout_binding.stageFlags     = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {ubo_layout_binding,
                                                                sampler_layout_binding};

        VkDescriptorSetLayoutCreateInfo layout_info = {};
        layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
        layout_info.pBindings    = bindings.data();

        if (vkCreateDescriptorSetLayout(_device, &layout_info, nullptr,
                                        &_descriptor_set_layout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor set layout");
        }
    }

    void _CreateDescriptorPool() {
        std::array<VkDescriptorPoolSize, 2> pool_size = {};
        /* MVC matrix */
        pool_size[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_size[0].descriptorCount = static_cast<uint32_t>(_swapchain_images.size());
        /* Obj Texture */
        pool_size[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        pool_size[1].descriptorCount = static_cast<uint32_t>(_swapchain_images.size());
        /* Cubemap */
        // pool_size[2].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        // pool_size[2].descriptorCount = static_cast<uint32_t>(_swapchain_images.size());
        // pool_size[3].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        // pool_size[3].descriptorCount = static_cast<uint32_t>(_swapchain_images.size());

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.poolSizeCount = static_cast<uint32_t>(pool_size.size());
        pool_info.pPoolSizes    = pool_size.data();
        pool_info.maxSets       = static_cast<uint32_t>(_swapchain_images.size());

        if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptor_pool) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to create descriptor pool");
        }
    }

    void _CreateDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(_swapchain_images.size(),
                                                   _descriptor_set_layout);

        VkDescriptorSetAllocateInfo alloc_info = {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = _descriptor_pool;
        alloc_info.descriptorSetCount = static_cast<uint32_t>(_swapchain_images.size());
        alloc_info.pSetLayouts        = layouts.data();

        _descriptor_sets.resize(_swapchain_images.size());
        if (vkAllocateDescriptorSets(_device, &alloc_info, _descriptor_sets.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate descriptor sets");
        }

        for (size_t i = 0; i < _swapchain_images.size(); i++) {
            VkDescriptorBufferInfo buffer_info = {};
            buffer_info.buffer                 = _uniform_buffers[i];
            buffer_info.offset                 = 0;
            buffer_info.range                  = sizeof(UniformBufferObject);

            VkDescriptorImageInfo image_info = {};
            image_info.imageLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            image_info.imageView             = _texture_img_view;
            image_info.sampler               = _texture_sampler;

            std::array<VkWriteDescriptorSet, 2> descriptor_write = {};
            descriptor_write[0].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write[0].dstSet          = _descriptor_sets[i];
            descriptor_write[0].dstBinding      = 0;
            descriptor_write[0].dstArrayElement = 0;
            descriptor_write[0].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptor_write[0].descriptorCount = 1;
            descriptor_write[0].pBufferInfo     = &buffer_info;

            descriptor_write[1].sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptor_write[1].dstSet          = _descriptor_sets[i];
            descriptor_write[1].dstBinding      = 1;
            descriptor_write[1].dstArrayElement = 0;
            descriptor_write[1].descriptorType =
                VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptor_write[1].descriptorCount = 1;
            descriptor_write[1].pImageInfo      = &image_info;

            /* ======== Cubemap ======== */
            // VkDescriptorBufferInfo cubemap_buffer = {};
            // buffer_info.buffer                    = _uniform_buffers_cubemap[i];
            // buffer_info.offset                    = 0;
            // buffer_info.range                     = sizeof(UniformBufferObject);

            // VkDescriptorImageInfo cubemap_image = {};
            // image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            // image_info.imageView   = _cubemap_img_view;
            // image_info.sampler     = _cubemap_sampler;

            // descriptor_write[2].sType           =
            // VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; descriptor_write[2].dstSet =
            // _descriptor_sets[i]; descriptor_write[2].dstBinding      = 2;
            // descriptor_write[2].dstArrayElement = 0;
            // descriptor_write[2].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // descriptor_write[2].descriptorCount = 1;
            // descriptor_write[2].pBufferInfo     = &cubemap_buffer;

            // descriptor_write[3].sType           =
            // VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET; descriptor_write[3].dstSet =
            // _descriptor_sets[i]; descriptor_write[3].dstBinding      = 3;
            // descriptor_write[3].dstArrayElement = 0;
            // descriptor_write[3].descriptorType =
            //     VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            // descriptor_write[3].descriptorCount = 1;
            // descriptor_write[3].pImageInfo      = &cubemap_image;

            vkUpdateDescriptorSets(_device,
                                   static_cast<uint32_t>(descriptor_write.size()),
                                   descriptor_write.data(), 0, nullptr);
        }
    }

    void _CreateDepthResources() {
        VkFormat depth_format = VK_FORMAT_D32_SFLOAT;
        _CreateImage(_swapchain_extent.width, _swapchain_extent.height, depth_format,
                     VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     1, 0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depth_image,
                     _depth_img_memory);
        _depth_img_view =
            _CreateImageView(_depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    VkFormat _FindSupportedFormat(const std::vector<VkFormat>& candidates,
                                  VkImageTiling tiling, VkFormatFeatureFlags features) {

        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(_physical_dev, format, &props);
        }
    }

    void _CreateImage(uint32_t width, uint32_t height, VkFormat format,
                      VkImageTiling tiling, VkImageUsageFlags usage, uint32_t layer_count,
                      VkImageCreateFlags flags, VkMemoryPropertyFlags properties,
                      VkImage& image, VkDeviceMemory& image_memory,
                      VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED) {
        VkImageCreateInfo image_info = {};
        image_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType         = VK_IMAGE_TYPE_2D;
        image_info.extent.width      = width;
        image_info.extent.height     = height;
        image_info.extent.depth      = 1;
        image_info.mipLevels         = 1;
        image_info.format            = format;
        image_info.tiling            = tiling;
        image_info.initialLayout     = initial_layout;
        image_info.usage             = usage;
        image_info.samples           = VK_SAMPLE_COUNT_1_BIT;
        image_info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        image_info.arrayLayers       = layer_count; /* For cubemaps */
        image_info.flags             = flags;       /* For cubemaps */

        if (vkCreateImage(_device, &image_info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Image");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(_device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize       = mem_requirements.size;
        alloc_info.memoryTypeIndex =
            _FindMemoryType(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(_device, &alloc_info, nullptr, &image_memory) !=
            VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate ImageMemory");
        }

        vkBindImageMemory(_device, image, image_memory, 0);
    }

    VkImageView _CreateImageView(VkImage image, VkFormat format,
                                 VkImageAspectFlagBits aspect_flags) {
        VkImageViewCreateInfo view_info       = {};
        view_info.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                       = image;
        view_info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format                      = format;
        view_info.subresourceRange.aspectMask = aspect_flags;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.levelCount     = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = 1;

        VkImageView image_view;
        if (vkCreateImageView(_device, &view_info, nullptr, &image_view) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view!");
        }

        return image_view;
    }

    stbi_uc* _LoadImage(const char* path, int& tex_width, int& tex_height,
                        VkDeviceSize& img_size) {
        int tex_channels;

        stbi_uc* pixel_buffer =
            stbi_load(path, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);

        img_size = tex_width * tex_height * 4; /* 4 bytes per pixel */

        if (!pixel_buffer) {
            throw std::runtime_error("Failed to load texture image");
        }

        return pixel_buffer;
    }
    gli::texture_cube _LoadCubemap(const char* path, int& tex_width, int& tex_height,
                                   VkDeviceSize& img_size) {
        gli::texture_cube tex_cube(gli::load(path));

        img_size   = tex_cube.size();
        tex_width  = tex_cube.extent().x;
        tex_height = tex_cube.extent().y;

        if (tex_cube.empty()) {
            throw std::runtime_error("Failed to load cubemap texture");
        }

        return tex_cube;
    }

    VkImage _CreateTextureImage(const char* path, VkDeviceMemory memory, int type) {
        VkImage texture;

        void*        img_data_buffer;
        int          tex_width, tex_height;
        VkDeviceSize img_size;

        gli::texture_cube tex_cube;

        if (type == 0) {
            img_data_buffer = _LoadImage(path, tex_width, tex_height, img_size);
        }
        if (type == 1) {
            tex_cube        = _LoadCubemap(path, tex_width, tex_height, img_size);
            img_data_buffer = tex_cube.data();
        }

        VkBuffer       staging_buffer;
        VkDeviceMemory staging_buffer_mem;
        _CreateBuffer(img_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                          VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                      staging_buffer, staging_buffer_mem);

        void* data;
        vkMapMemory(_device, staging_buffer_mem, 0, img_size, 0, &data);
        memcpy(data, img_data_buffer, static_cast<size_t>(img_size));
        vkUnmapMemory(_device, staging_buffer_mem);

        if (type == 0) {
            stbi_image_free(img_data_buffer);
            _CreateImage(tex_width, tex_height, VK_FORMAT_R8G8B8A8_UNORM,
                         VK_IMAGE_TILING_OPTIMAL,
                         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 1,
                         0, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture, memory);

            _TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            _CopyBufferToImage(staging_buffer, texture, static_cast<uint32_t>(tex_width),
                               static_cast<uint32_t>(tex_height));

            _TransitionImageLayout(texture, VK_FORMAT_R8G8B8A8_UNORM,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        if (type == 1) {
            _CreateImage(tex_width, tex_height, VK_FORMAT_BC3_UNORM_BLOCK,
                         VK_IMAGE_TILING_OPTIMAL,
                         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 6,
                         VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture, memory);

            _TransitionImageLayout(texture, VK_FORMAT_BC3_UNORM_BLOCK,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 6);

            _CopyBufferToImage(staging_buffer, texture, static_cast<uint32_t>(tex_width),
                               static_cast<uint32_t>(tex_height), 6, tex_cube[0].size());

            _TransitionImageLayout(texture, VK_FORMAT_BC3_UNORM_BLOCK,
                                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 6);
        }

        vkDestroyBuffer(_device, staging_buffer, nullptr);
        vkFreeMemory(_device, staging_buffer_mem, nullptr);

        return texture;
    }

    VkImageView _CreateTextureImageView(VkImage  texture,
                                        VkFormat format = VK_FORMAT_R8G8B8A8_UNORM) {
        VkImageView img_view =
            _CreateImageView(texture, format, VK_IMAGE_ASPECT_COLOR_BIT);
        return img_view;
    }
    VkSampler _CreateTextureSampler() {
        VkSampler           sampler;
        VkSamplerCreateInfo sampler_info     = {};
        sampler_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter               = VK_FILTER_LINEAR;
        sampler_info.magFilter               = VK_FILTER_LINEAR;
        sampler_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable        = VK_TRUE;
        sampler_info.maxAnisotropy           = 16;
        sampler_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable           = VK_FALSE;
        sampler_info.compareEnable           = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias              = 0.f;
        sampler_info.minLod                  = 0.f;
        sampler_info.maxLod                  = 0.f;

        if (vkCreateSampler(_device, &sampler_info, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create texture sampler");
        }
        return sampler;
    }

    VkCommandBuffer _BeginSingleTimeCommands() {
        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool        = _command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(_device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);

        return command_buffer;
    }

    void _EndSingleTimeCommands(VkCommandBuffer command_buffer) {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info       = {};
        submit_info.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers    = &command_buffer;

        vkQueueSubmit(_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(_graphics_queue);

        vkFreeCommandBuffers(_device, _command_pool, 1, &command_buffer);
    }

    void _TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout old_layout,
                                VkImageLayout new_layout, uint32_t layer_count = 1) {
        VkCommandBuffer command_buffer = _BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier_info        = {};
        barrier_info.sType                       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier_info.oldLayout                   = old_layout;
        barrier_info.newLayout                   = new_layout;
        barrier_info.srcQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        barrier_info.dstQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        barrier_info.image                       = image;
        barrier_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier_info.subresourceRange.baseArrayLayer = 0;
        barrier_info.subresourceRange.levelCount     = 1;
        barrier_info.subresourceRange.layerCount     = layer_count;
        barrier_info.srcAccessMask                   = 0; /* TODO */
        barrier_info.dstAccessMask                   = 0; /* TODO */

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (old_layout == VK_IMAGE_LAYOUT_BEGIN_RANGE &&
            new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier_info.srcAccessMask = 0;
            barrier_info.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier_info.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier_info.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("Unsupported layout transition");
        }

        vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier_info);

        _EndSingleTimeCommands(command_buffer);
    }

  public:
    VkDevice GetDevice() { return _device; }

  private:
    GLFWwindow*                  _window;
    VkInstance                   _instance;
    VkDebugUtilsMessengerEXT     _debug_messenger;
    VkSurfaceKHR                 _surface;
    VkPhysicalDevice             _physical_dev = VK_NULL_HANDLE;
    VkDevice                     _device;
    VkQueue                      _graphics_queue;
    VkQueue                      _present_queue;
    VkSwapchainKHR               _swapchain;
    std::vector<VkImage>         _swapchain_images;
    VkFormat                     _swapchain_img_format;
    VkExtent2D                   _swapchain_extent;
    std::vector<VkImageView>     _swapchain_img_views;
    VkImage                      _depth_image;
    VkDeviceMemory               _depth_img_memory;
    VkImageView                  _depth_img_view;
    VkImage                      _texture_image;
    VkDeviceMemory               _texture_img_memory;
    VkImageView                  _texture_img_view;
    VkSampler                    _texture_sampler;
    VkImage                      _cubemap_image;
    VkDeviceMemory               _cubemap_img_memory;
    VkImageView                  _cubemap_img_view;
    VkSampler                    _cubemap_sampler;
    VkRenderPass                 _renderpass;
    VkPipelineLayout             _pipeline_layout;
    VkPipeline                   _graphics_pipeline;
    std::vector<VkFramebuffer>   _swapchain_framebuffers;
    VkCommandPool                _command_pool;
    std::vector<VkCommandBuffer> _command_buffers;
    std::vector<VkSemaphore>     _semaphores_img_available;
    std::vector<VkSemaphore>     _semaphores_render_finished;
    std::vector<VkFence>         _fences_inflight;
    size_t                       _current_frame = 0;
    std::vector<Vertex>          _vertices;
    std::vector<uint32_t>        _indices;
    VkBuffer                     _vertex_buffer;
    VkDeviceMemory               _vertex_buffer_memory;
    VkBuffer                     _index_buffer;
    VkDeviceMemory               _index_buffer_memory;
    std::vector<VkBuffer>        _uniform_buffers;
    std::vector<VkDeviceMemory>  _uniform_buffers_memory;
    std::vector<VkBuffer>        _uniform_buffers_cubemap;
    std::vector<VkDeviceMemory>  _uniform_buffers_cubemap_memory;
    VkDescriptorSetLayout        _descriptor_set_layout;
    VkDescriptorPool             _descriptor_pool;
    std::vector<VkDescriptorSet> _descriptor_sets;
    VkDescriptorSet              _descriptor_skybox;
};