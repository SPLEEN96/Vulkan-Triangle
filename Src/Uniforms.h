#pragma once
#include "Buffer.h"
#include "HelperFunctions.h"
#include "RenderAllocator.h"

#include <glm/glm.hpp>

#include <array>

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

/*
 * Descriptor pool : Descriptor set are allocated from the pool.
 */

/*
 * Descriptor Layout : specifies the type of ressources
 * that are going to be access by the pipeline.
 */
void CreateDescriptorLayout(const VkDevice& device, VkDescriptorSetLayout& target) {
    /* Details about shader binding --> latout(binding = 0) */
    VkDescriptorSetLayoutBinding mvp_binding = {};
    mvp_binding.binding                      = 0; /* Binding used in the shader */
    mvp_binding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    mvp_binding.descriptorCount              = 1; /* Number of vals in the array */
    mvp_binding.stageFlags                   = VK_SHADER_STAGE_VERTEX_BIT;
    mvp_binding.pImmutableSamplers           = nullptr; /* For image samplers */

    VkDescriptorSetLayoutBinding light_binding = {};
    light_binding.binding                      = 1;
    light_binding.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    light_binding.descriptorCount              = 1;
    light_binding.stageFlags                   = VK_SHADER_STAGE_FRAGMENT_BIT;
    light_binding.pImmutableSamplers           = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 2> ubo_bindings = {mvp_binding,
                                                                light_binding};

    VkDescriptorSetLayoutCreateInfo ubo_layout_info = {};
    ubo_layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ubo_layout_info.bindingCount = 2;
    ubo_layout_info.pBindings    = ubo_bindings.data();

    VK_ASSERT(vkCreateDescriptorSetLayout(nullptr, &ubo_layout_info, nullptr, &target),
              "Failed to create descriptor set layout");

    /* Bind the Set Layouts to the Pipeline Layout */
}

/*
 * Descriptor set : specifies the buffer or image
 * that will be bound to the descriptors.
 * (One Set List for each UBOs)
 * (List size = N Images)
 * (The sets are bounds for the drawing command)
 */
void AllocateDescriptorSets(const VkDevice& device, size_t img_count,
                            VkDescriptorSetLayout         layout,
                            std::vector<VkDescriptorSet>& target) {
    /* Because pSetLayouts has to be the same size of the descriptorSetCount */
    std::vector<VkDescriptorSetLayout> layouts(img_count, layout);

    VkDescriptorSetAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // alloc_info.descriptorPool     = descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32>(layouts.size);
    alloc_info.pSetLayouts        = layouts.data();

    target.resize(img_count);
    VK_ASSERT(vkAllocateDescriptorSets(device, &alloc_info, target.data()),
              "Failed to allocate Descriptor Sets");
}

void ConfigureDescriptorSets(const VkDevice& device, size_t img_count,
                             std::vector<Backend::Buffer> buffers, uint32 binding,
                             std::vector<VkDescriptorSet>& target) {
    for (size_t i = 0; i < img_count; i++) {
        VkDescriptorBufferInfo buffer_info = {};
        buffer_info.buffer                 = buffers[i].Buffer;
        buffer_info.offset                 = 0;
        buffer_info.range                  = buffers[i].Size;

        VkWriteDescriptorSet descriptor_write = {};
        descriptor_write.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptor_write.dstSet               = target[i];
        descriptor_write.dstBinding           = binding;
        descriptor_write.dstArrayElement      = 0;
        descriptor_write.descriptorType       = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptor_write.descriptorCount      = 1;
        descriptor_write.pBufferInfo          = &buffer_info;
        descriptor_write.pImageInfo           = nullptr; /* For img data */
        descriptor_write.pTexelBufferView     = nullptr; /* For buffer views */

        vkUpdateDescriptorSets(device, 1, &descriptor_write, 0, nullptr);
    }
}

/* Create the Uniform Buffer
 * (One Buffer List for each UBOs)
 * (List size = N Images)
 */
void CreateUniformBuffers(const VkDevice& device, size_t img_count,
                          std::vector<Backend::Buffer>& target) {
    VkDeviceSize buffer_size = sizeof(UniformBufferObject);
    target.resize(img_count);

    for (size_t i = 0; i < img_count; i++) {
        Backend::AllocateBuffer(device, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                target[i]);
    }
}

/* Update the Uniform Buffer */
void UpdateUniformBuffer(uint32 curr_img_index, std::vector<Backend::Buffer>& target) {
    UniformBufferObject ubo = {};
    ubo.model               = glm::mat4(1.f);
    ubo.view                = glm::mat4(1.f);
    ubo.projection          = glm::mat4(1.f);

    target[curr_img_index].MapMemory();
    target[curr_img_index].CopyMemory(&ubo, sizeof(ubo));
    target[curr_img_index].UnmapMemory();
}

class DynamicUniform {};
