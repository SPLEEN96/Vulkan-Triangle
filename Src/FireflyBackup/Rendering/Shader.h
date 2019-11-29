#pragma once
#include "RENDER_CORE.h"

namespace Firefly {
namespace Rendering {
namespace Shader {

struct Shader {
    VkShaderModule    Module;
    std::vector<char> Buffer;
};

struct Vertex {
    static VkVertexInputBindingDescription                  GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 3> GetAttribDescription();
};

void CreateShader(VkDevice device, const std::string& path, Shader& target);
void CreateShaderModule(VkDevice device, Shader& target);
void ReadShaderFileToBuffer(const std::string& path, Shader& target);

} // namespace Shader
} // namespace Rendering
} // namespace Firefly