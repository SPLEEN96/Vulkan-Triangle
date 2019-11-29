#pragma once
#include "Rendering/RENDER_CORE.h"

namespace Firefly {
namespace Rendering {
namespace Presentation {

struct FramebufferAttachment {
    VkDeviceMemory Memory;
    VkImage        Image;
    VkImageView    View;
    VkFormat       Format;
};
struct Framebuffer {
    uint32                Width, Height;
    VkFramebuffer         Framebuffer;
    FramebufferAttachment Color;
    FramebufferAttachment Depth;
    VkRenderPass          RenderPass;
};
struct Gbuffer {
    uint32                Width, Height;
    VkFramebuffer         Framebuffer;
    FramebufferAttachment Position, Normal, Albedo;
    FramebufferAttachment Depth;
    VkRenderPass          RenderPass;
};

void CreateFramebuffer(VkDevice device, VkRenderPass renderpass, VkExtent2D extent,
                       FramebufferAttachment color_attachment,
                       FramebufferAttachment depth_attachment, Framebuffer& target);

void CreateAttachment(VkDevice device, VkExtent2D extent, VkImage* source,
                      VkFormat format, VkImageUsageFlagBits usage_flag,
                      FramebufferAttachment& target);

void CreateImage(VkDevice device, VkExtent2D extent, VkFormat format,
                 VkImageUsageFlagBits usage, VkMemoryPropertyFlags properties,
                 FramebufferAttachment& target);

void CreateImageView(VkDevice device, VkImage source, VkFormat format,
                     VkImageAspectFlagBits aspect_mask, VkImageView& target);

} // namespace Presentation
} // namespace Rendering
} // namespace Firefly