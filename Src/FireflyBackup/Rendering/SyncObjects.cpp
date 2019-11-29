#include "SyncObjects.h"

namespace Firefly {
namespace Rendering {
namespace SyncObjects {

void CreateSemaphore(const VkDevice& device, VkSemaphore& target) {
    VkSemaphoreCreateInfo create_info = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VK_ASSERT(vkCreateSemaphore(device, &create_info, nullptr, &target),
              "Failed to create Semaphore.");
}

void CreateFence(const VkDevice& device, VkFence& target) {
    VkFenceCreateInfo create_info = {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    create_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_ASSERT(vkCreateFence(device, &create_info, nullptr, &target),
              "Failed to create Fence.");
}

} // namespace SyncObjects
} // namespace Rendering
} // namespace Firefly