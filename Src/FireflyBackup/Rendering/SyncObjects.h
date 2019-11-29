#pragma once

#include "RenderBackend.h"

namespace Firefly {
namespace Rendering {
namespace SyncObjects {

void CreateSemaphore(const VkDevice& device, VkSemaphore& target);
void CreateFence(const VkDevice& device, VkFence& target);

} // namespace SyncObjects
} // namespace Rendering
} // namespace Firefly
