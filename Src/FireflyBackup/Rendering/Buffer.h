#pragma once
#include "vulkan/vulkan.h"

#include <memory.h>
#include <vector>

namespace Firefly {
namespace RenderBackend {
struct Buffer {
    VkDevice               Device;
    VkBuffer               Buffer = VK_NULL_HANDLE;
    VkDeviceMemory         Memory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo Descriptor;
    VkDeviceSize           Size         = 0;
    VkDeviceSize           Alignment    = 0;
    void*                  MappedMemory = nullptr;

    VkBufferUsageFlags    UsageFlags;       /* ex: Uniforms, Transfer, etc */
    VkMemoryPropertyFlags MemPropertyFlags; /* ex: Host coherent bit */

    /*
     * Bind the allocated DeviceMemory block to the buffer
     * @param offset (Optional) : Byte offset from the beginning of the memory block
     */
    VkResult Bind(VkDeviceSize offset = 0) {
        return vkBindBufferMemory(Device, Buffer, Memory, offset);
    }

    /*
     * Map a memory range to MappedMemory if successful
     * @param size (Optional) : VK_WHOLE_SIZE to map the complete buffer range
     * @param offset (Optional) : Byte offset from the beginning
     */
    VkResult MapMemory(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
        return vkMapMemory(Device, Memory, offset, Size, 0, &MappedMemory);
    }

    /*
     * Unmap MappedMemory (Can't fail)
     */
    void UnmapMemory() {
        if (MappedMemory) {
            vkUnmapMemory(Device, Memory);
        }
    }

    /*
     * Copy data to MappedMemory
     * @param data_src : The data to be copied
     * @param size : Size of the data to be copied
     */
    void CopyMemory(void* data_src, VkDeviceSize size) {
        if (MappedMemory) {
            memcpy(MappedMemory, data_src, size);
        }
    }

    /*
     *
     */
    void SetupDescriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) {
        Descriptor.offset = offset;
        Descriptor.buffer = Buffer;
        Descriptor.range  = size;
    }

    /*
     * Free all Vulkan Resources in this buffer
     */
    void Destroy() {
        if (Buffer) {
            vkDestroyBuffer(Device, Buffer, nullptr);
        }
        if (Memory) {
            vkFreeMemory(Device, Memory, nullptr);
        }
    }
};
} // namespace RenderBackend
} // namespace Firefly