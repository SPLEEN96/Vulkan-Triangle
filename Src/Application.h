#pragma once

// #define VK_USE_PLATFORM_XCB_KHR
// #include <vulkan/vulkan.h>
// #include <vulkan/vulkan_xcb.h>
//#define GLFW_EXPOSE_NATIVE_X11
//#define GLFW_EXPOSE_NATIVE_GLX

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

const int WIDTH  = 800;
const int HEIGHT = 600;

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

class Application {
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
        while (!glfwWindowShouldClose(_window)) {
            glfwPollEvents();
            glfwSwapBuffers(_window);
        }
    }

    void _Cleanup() {
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
        return indices.IsComplete() && extensions_supported && swapchain_adequate;
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
        /* Resolution of images in swapchain */
        VkExtent2D extent = _ChooseSwapExtent(swapchain_support.capabilities);

        uint32_t image_count = swapchain_support.capabilities.minImageCount + 2;
        /* If maxImageCount ==0, there is no maximum */
        if (swapchain_support.capabilities.maxImageCount > 0 &&
            image_count > swapchain_support.capabilities.maxImageCount) {
            image_count = swapchain_support.capabilities.maxImageCount;
        }

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
            create_info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices   = queue_familiy_indices;
        } else {
            create_info.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
            create_info.queueFamilyIndexCount = 0;       /* Optional */
            create_info.pQueueFamilyIndices   = nullptr; /* Optional */
        }

        /* To specify that no transforms (clockwise rotation, etc) is applied to images in
         * swapchain */
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
        VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& available_present_mode : available_present_modes) {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return available_present_mode;
            } else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                best_mode = available_present_mode;
            }
        }
        return best_mode;
    }

    VkExtent2D _ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        /* If currentExtent.width = max of uint32_t, the resolution of the swap
         * chain is not the same of the window */
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
            VkImageViewCreateInfo create_info = {};
            create_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image                 = _swapchain_images[i];
            create_info.viewType              = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format                = _swapchain_img_format;
            create_info.components.r          = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g          = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b          = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a          = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel   = 0;
            create_info.subresourceRange.levelCount     = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(_device, &create_info, nullptr,
                                  &_swapchain_img_views[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views");
            }
        }
    }

  private:
    GLFWwindow*              _window;
    VkInstance               _instance;
    VkDebugUtilsMessengerEXT _debug_messenger;
    VkSurfaceKHR             _surface;
    VkPhysicalDevice         _physical_dev = VK_NULL_HANDLE;
    VkDevice                 _device;
    VkQueue                  _graphics_queue;
    VkQueue                  _present_queue;
    VkSwapchainKHR           _swapchain;
    std::vector<VkImage>     _swapchain_images;
    VkFormat                 _swapchain_img_format;
    VkExtent2D               _swapchain_extent;
    std::vector<VkImageView> _swapchain_img_views;
};