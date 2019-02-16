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
    VkPipeline*        Pipeline; /* Pointer to the pipeline used by this material */
};

struct Light {
    glm::vec4 Position;
    glm::vec3 Color;
    float     radius;
};

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
    } _offscreen_framebuffer;

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
            aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            img_layout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        if (aspect_mask == 0) {
            throw std::runtime_error("Attachment usage not supported");
        }

        _app._CreateImage(_offscreen_framebuffer.Width, _offscreen_framebuffer.Height,
                          format, VK_IMAGE_TILING_OPTIMAL, usage, 1, 0,
                          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, attachment->Image,
                          attachment->Memory);

        attachment->View =
            _app._CreateImageView(attachment->Image, attachment->Format, aspect_mask);
    }

    /*
     * Initialize a new framebuffer and attachments for offscreen rendering (G-Buffer)
     */
    void _PrepareOffscreenFramebuffer() {
        _offscreen_framebuffer.Width  = FRAMEBUFFER_DIMENSION;
        _offscreen_framebuffer.Height = FRAMEBUFFER_DIMENSION;

        /* Positions */
        _CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_offscreen_framebuffer.Position);

        /* Normals */
        _CreateAttachment(VK_FORMAT_R16G16B16A16_SFLOAT,
                          VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_offscreen_framebuffer.Normal);

        /* Albedo */
        _CreateAttachment(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                          &_offscreen_framebuffer.Albedo);
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