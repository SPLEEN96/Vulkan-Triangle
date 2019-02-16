#include "Application.h"
#include "Buffer.h"

#include <stdexcept>

typedef uint32_t uint32;
typedef int32_t  int32;

#define TEX_DIMENSION 2048
#define TEX_FILTER VK_FILTER_LINEAR

#define FRAMEBUFFER_DIMENSION TEX_DIMENSION

struct SVertex {
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec3 Color;
    // glm::vec2 Uv;
};

struct MaterialProperties {
    glm::vec4 Ambient;
    glm::vec4 Diffuse;
    glm::vec4 Specular;
    float     Opacity;
};

struct Material {
    std::string        Name;
    MaterialProperties Properties;
    VkDescriptorSet    DescriptorSet;
    VkPipeline* Pipeline; /* Pointer to the pipeline used by this material */
};

struct Light {
    glm::vec4 Position;
    glm::vec3 Color;
    float     radius;
};

#define VK_ASSERT(x, ...)                                                      \
    {                                                                          \
        if (x != VK_SUCCESS) {                                                 \
            throw std::runtime_error(__VA_ARGS__);                             \
        }                                                                      \
    }

/* CLASS */
class DeferredRenderer {
  private:
    /* Used for offscreen rendering */
    struct FramebufferAttachment {
        VkImage        Image;
        VkDeviceMemory Memory;
        VkImageView    View;
        VkFormat       Format;
    };

    struct Framebuffer {
        int32                 Width, Height;
        VkFramebuffer         Framebuffer;
        FramebufferAttachment Position, Normal, Albedo;
        FramebufferAttachment Depth;
        VkRenderPass          Renderpass;
    } _gbuffer;

  public:
    DeferredRenderer() {
        _app._InitWindow();
        _app._InitVulkan();
    }

  private:
    /*
     *
     * @param format : FORMAT_R8G8B8_UNORM, etc
     * @param usage : Either a Color or a Depth attachment
     * @param attachment : Handle to the attachment to be created
     */
    void _CreateAttachment(VkFormat format, VkImageUsageFlagBits usage,
                           FramebufferAttachment* attachment) {
        VkImageAspectFlags aspect_mask = 0;
        VkImageLayout      img_layout;

        attachment->Format = format;
        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
            aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
            img_layout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            aspect_mask =
                VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            img_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        if (aspect_mask == 0) {
            throw std::runtime_error("Attachment usage not supported");
        }

        _app._CreateImage(_gbuffer.Width, _gbuffer.Height, format,
                          VK_IMAGE_TILING_OPTIMAL, usage, 1, 0,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                          attachment->Image, attachment->Memory);

        attachment->View = _app._CreateImageView(
            attachment->Image, attachment->Format, aspect_mask);
    }

    /*
     * Initialize a new framebuffer and attachments for offscreen rendering
     * (G-Buffer)
     */
    void _PrepareOffscreenFramebuffer() {
        _gbuffer.Width  = FRAMEBUFFER_DIMENSION;
        _gbuffer.Height = FRAMEBUFFER_DIMENSION;

        /* === COLOR === */

        /* Positions */
        _CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_gbuffer.Position);

        /* Normals */
        _CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_gbuffer.Normal);

        /* Albedo */
        _CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_gbuffer.Albedo);

        _CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_gbuffer.Albedo);

        /* === DEPTH === */

        /* TODO: Find valid formats */
        VkFormat attachment_depth_format = VK_FORMAT_D32_SFLOAT;
        _CreateAttachment(attachment_depth_format,
                          VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                          &_gbuffer.Depth);

        /* === RENDERPASS === */

        /* Attachments Properties */
        std::array<VkAttachmentDescription, 4> attachment_descrpts = {};

        for (uint32 i = 0; i < 4; i++) {
            attachment_descrpts[i].samples = VK_SAMPLE_COUNT_1_BIT;
            attachment_descrpts[i].loadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment_descrpts[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_descrpts[i].stencilLoadOp =
                VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_descrpts[i].stencilStoreOp =
                VK_ATTACHMENT_STORE_OP_DONT_CARE;

            /* if depth attachment */
            if (i == 3) {
                attachment_descrpts[i].initialLayout =
                    VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descrpts[i].finalLayout =
                    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            } else {
                attachment_descrpts[i].initialLayout =
                    VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descrpts[i].finalLayout =
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }

        attachment_descrpts[0].format = _gbuffer.Position.Format;
        attachment_descrpts[1].format = _gbuffer.Normal.Format;
        attachment_descrpts[2].format = _gbuffer.Albedo.Format;
        attachment_descrpts[3].format = _gbuffer.Depth.Format;

        /* Attachment References */
        std::vector<VkAttachmentReference> color_attachment_refs;
        color_attachment_refs.push_back(
            {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        color_attachment_refs.push_back(
            {1, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        color_attachment_refs.push_back(
            {2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});

        VkAttachmentReference depth_ref = {};
        depth_ref.attachment            = 3;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        /* Subpass */
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments    = color_attachment_refs.data();
        subpass.colorAttachmentCount =
            static_cast<uint32>(color_attachment_refs.size());
        subpass.pDepthStencilAttachment = &depth_ref;

        /* Attachment Layout transitions */
        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass   = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass   = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask   = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        /* Renderpass */
        VkRenderPassCreateInfo renderpass_info = {};
        renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderpass_info.pAttachments = attachment_descrpts.data();
        renderpass_info.attachmentCount =
            static_cast<uint32>(attachment_descrpts.size());
        renderpass_info.subpassCount    = 1;
        renderpass_info.pSubpasses      = &subpass;
        renderpass_info.dependencyCount = 2;
        renderpass_info.pDependencies   = dependencies.data();

        vkCreateRenderPass(_app._device, &renderpass_info, nullptr,
                           &_gbuffer.Renderpass);
    }

  private:
    // struct{
    //     Model model;
    //     Model floor;
    //     Model quad;
    // }models;
    // struct {
    //     struct {
    //         Texture2D ColorMap;
    //         Texture2D NormalMap;
    //     } model;
    //     struct {
    //         Texture2D ColorMap;
    //         Texture2D NormalMap;
    //     } floor;
    // } texture;
    Application _app;

    struct {
        VkPipelineVertexInputStateCreateInfo           InputeState;
        std::vector<VkVertexInputBindingDescription>   BindingDescriptions;
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions;
    } _vertices;

    struct {
        Light     Lights[6];
        glm::vec4 ViewPos;
    } _ubo_frag_lights;

    struct {
        Backend::Buffer vertFullscreen;
        Backend::Buffer vertOffscreen;
        Backend::Buffer fragLights;
    } _uniform_buffers;

    struct {
        VkPipeline Deferred;
        VkPipeline Offscreen;
        VkPipeline Debug;
    } _pipelines;

    struct {
        VkPipelineLayout Defered;
        VkPipelineLayout Offscreen;
    } _pipeline_layouts;

    struct {
        VkDescriptorSet model;
        VkDescriptorSet floor;
    } _descriptor_sets;

    VkDescriptorSetLayout descriptor_layout;

    /* Sampler for the framebuffer color attachments */
    VkSampler _color_sampler;

    VkCommandBuffer _offscreen_cmd_buffer = VK_NULL_HANDLE;
};

//==========SCENE=================
// VkDevice         _device;
// VkQueue          _queue;
// VkDescriptorPool _descriptor_pool;

// struct {
//     VkDescriptorSetLayout material;
//     VkDescriptorSetLayout scene;
// } _descriptor_set_layouts;

// VkDescriptorSet _descriptor_scene;

// Backend::Buffer _vertex_buffer;
// Backend::Buffer _index_buffer;

// std::vector<Material> materials;