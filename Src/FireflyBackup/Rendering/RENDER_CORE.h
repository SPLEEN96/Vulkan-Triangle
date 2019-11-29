#pragma once
#include "FFLY_CORE.h"

#include "vulkan/vulkan.h"

namespace Firefly {
namespace Rendering {

#define VK_ASSERT(x, ...)                                                               \
    {                                                                                   \
        if (x != VK_SUCCESS) {                                                          \
            FFLY_LOG_CORE_CRITICAL("VULKAN::{0}", __VA_ARGS__);                         \
            *(int*)0 = 0;                                                               \
        }                                                                               \
    }

} // namespace Rendering
} // namespace Firefly
