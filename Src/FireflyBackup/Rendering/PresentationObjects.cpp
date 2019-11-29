#include "Rendering/PresentationObjects.h"

namespace Firefly {
namespace Rendering {
namespace Presentation {

void CreateFramebuffer(VkDevice device, VkRenderPass renderpass, VkExtent2D extent,
                       FramebufferAttachment color_attachment,
                       FramebufferAttachment depth_attachment, Framebuffer& target) {
    target.Color      = color_attachment;
    target.Depth      = depth_attachment;
    target.Width      = extent.width;
    target.Height     = extent.height;
    target.RenderPass = renderpass;

    std::array<VkImageView, 2> attachments = {target.Color.View, target.Depth.View};

    VkFramebufferCreateInfo create_info = {VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};
    create_info.renderPass              = target.RenderPass;
    create_info.attachmentCount         = static_cast<uint32>(attachments.size());
    create_info.pAttachments            = attachments.data();
    create_info.width                   = target.Width;
    create_info.height                  = target.Height;
    create_info.layers                  = 1;

    VK_ASSERT(vkCreateFramebuffer(device, &create_info, nullptr, &target.Framebuffer),
              "Failed to create Framebuffer.");
}

void CreateAttachment(VkDevice device, VkExtent2D extent, VkImage* source,
                      VkFormat format, VkImageUsageFlagBits usage,
                      FramebufferAttachment& target) {
    VkImageAspectFlagBits aspect_mask = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
    VkImageLayout         img_layout;

    target.Format = format;
    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
        img_layout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    FFLY_ASSERT((aspect_mask < VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM),
                "Attachment usage not supported");

    /* Use swapchain image */
    if (source != nullptr) {
        target.Image = *source;
    }
    /* Use custom image */
    else {
        CreateImage(device, extent, format, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    target);
    }

    CreateImageView(device, target.Image, format, aspect_mask, target.View);
}

void CreateImage(VkDevice device, VkExtent2D extent, VkFormat format,
                 VkImageUsageFlagBits usage, VkMemoryPropertyFlags properties,
                 FramebufferAttachment& target) {
    VkImageCreateInfo create_info = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    create_info.imageType         = VK_IMAGE_TYPE_2D;
    create_info.extent.width      = extent.width;
    create_info.extent.height     = extent.height;
    create_info.extent.depth      = 1;
    create_info.mipLevels         = 1;
    create_info.format            = format;
    create_info.tiling            = VK_IMAGE_TILING_OPTIMAL;
    create_info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    create_info.usage             = usage;
    create_info.samples           = VK_SAMPLE_COUNT_1_BIT;
    create_info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    create_info.arrayLayers       = 1;

    VK_ASSERT(vkCreateImage(device, &create_info, nullptr, &target.Image),
              "Failed to create Image.");

    VkMemoryRequirements mem_requirements;
    vkGetImageMemoryRequirements(device, target.Image, &mem_requirements);

    /* TODO */
    VkMemoryAllocateInfo alloc_info = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    alloc_info.allocationSize       = mem_requirements.size;
    // alloc_info.memoryTypeIndex=

    VK_ASSERT(vkAllocateMemory(device, &alloc_info, nullptr, &target.Memory),
              "Failed to allocate Image Memory.");

    vkBindImageMemory(device, target.Image, target.Memory, 0);
}

void CreateImageView(VkDevice device, VkImage source, VkFormat format,
                     VkImageAspectFlagBits aspect_mask, VkImageView& target) {
    VkImageViewCreateInfo create_info       = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
    create_info.image                       = source;
    create_info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
    create_info.format                      = format;
    create_info.subresourceRange.aspectMask = aspect_mask;
    create_info.subresourceRange.baseMipLevel   = 0;
    create_info.subresourceRange.levelCount     = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount     = 1;

    VK_ASSERT(vkCreateImageView(device, &create_info, nullptr, &target),
              "Failed to create Image View.");
}

} // namespace Presentation
} // namespace Rendering
} // namespace Firefly