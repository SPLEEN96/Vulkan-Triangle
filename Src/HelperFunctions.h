#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.h>

typedef uint32_t uint32;
typedef int32_t  int32;

#define VK_ASSERT(x, ...)                                                               \
    {                                                                                   \
        if (x != VK_SUCCESS) {                                                          \
            throw std::runtime_error(__VA_ARGS__);                                      \
        }                                                                               \
    }

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

VkPipelineShaderStageCreateInfo LoadShader(const char*           filepath,
                                           VkShaderStageFlagBits shader_stage, VkDevice device) {
    
    auto                     shdcode     = ReadFile(filepath);

    VkShaderModuleCreateInfo module_info = {};
    module_info.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    module_info.codeSize                 = shdcode.size();
    module_info.pCode = reinterpret_cast<const uint32*>(shdcode.data());

    VkShaderModule shader_module;
    VK_ASSERT(vkCreateShaderModule(device, &module_info, nullptr, &shader_module),
              "Failed to create shader module");

    VkPipelineShaderStageCreateInfo stage_info={};
    stage_info.sType=VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_info.stage=shader_stage;
    stage_info.module=shader_module;
    stage_info.pName="main";

    return stage_info;
}
