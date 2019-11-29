#pragma once
#include "RENDER_CORE.h"

namespace Firefly {
namespace Rendering {
namespace GUI {

class GUIRenderer {
  public:
    void Init();
    void Update();
    void Render();

  private:
    VkSampler             sampler;
    //vks::Buffer           vertexBuffer;
   // vks::Buffer           indexBuffer;
    int32_t               vertexCount = 0;
    int32_t               indexCount  = 0;
    VkDeviceMemory        fontMemory  = VK_NULL_HANDLE;
    VkImage               fontImage   = VK_NULL_HANDLE;
    VkImageView           fontView    = VK_NULL_HANDLE;
    VkPipelineCache       pipelineCache;
    VkPipelineLayout      pipelineLayout;
    VkPipeline            pipeline;
    VkDescriptorPool      descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet       descriptorSet;
    //vks::VulkanDevice*    device;
    //VulkanExampleBase*    example;
};

} // namespace GUI
} // namespace Rendering
} // namespace Firefly