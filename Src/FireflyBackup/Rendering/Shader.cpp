#include "Shader.h"
#include "PCH_CORE.h"

namespace Firefly {
namespace Rendering {
namespace Shader {

void CreateShader(VkDevice device, const std::string& file_path, Shader& target) {
    ReadShaderFileToBuffer(file_path, target);
    CreateShaderModule(device, target);
}

void CreateShaderModule(VkDevice device, Shader& target) {
    VkShaderModuleCreateInfo create_info = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    create_info.codeSize                 = target.Buffer.size();
    create_info.pCode = reinterpret_cast<const uint32*>(target.Buffer.data());

    VK_ASSERT(vkCreateShaderModule(device, &create_info, nullptr, &target.Module),
              "Failed to create Shader Module.");
}

void ReadShaderFileToBuffer(const std::string& file_path, Shader& target) {
    std::ifstream file(file_path, std::ios::ate | std::ios::binary);

    FFLY_ASSERT(file.is_open(), "Failed to open Shader File.");

    size_t file_size = (size_t)file.tellg();
    target.Buffer.resize(file_size);

    file.seekg(0);
    file.read(target.Buffer.data(), file_size);

    file.close();
}

VkVertexInputBindingDescription Vertex::GetBindingDescription() {
    VkVertexInputBindingDescription bloink;
    return bloink;
}
std::array<VkVertexInputAttributeDescription, 3> Vertex::GetAttribDescription() {
    std::array<VkVertexInputAttributeDescription, 3> bloup;
    return bloup;
}

} // namespace Shader
} // namespace Rendering
} // namespace Firefly