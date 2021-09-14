//
// Created by jack on 7/1/21.
//

#ifndef PIPERINE_INFO_TYPES_H
#define PIPERINE_INFO_TYPES_H

#include "vulkan_types.h"

#include <map>
#include <optional>

#include <glm/glm.hpp>

#ifndef PIPERINE_KEY_TYPE
#define PIPERINE_KEY_TYPE size_t
#endif

namespace pip {

    struct ShaderBinding {
        VkVertexInputBindingDescription binding;
        std::vector<VkVertexInputAttributeDescription> attributes;
    };

    struct ShaderDescriptor {
        std::filesystem::path path;
        ShaderType type;
        std::vector<ShaderBinding> bindings;
        std::optional<uint32_t> pushConstantSize;
        std::optional<uint32_t> uniformSize;
    };

    struct RenderPassDescriptor {

    };

    struct PipelineDescriptor {
        PIPERINE_KEY_TYPE vertexShaderKey;
        PIPERINE_KEY_TYPE fragmentShaderKey;

        RenderPassDescriptor renderPass;
    };

    struct PipelineGraph {
        std::map<PIPERINE_KEY_TYPE, ShaderDescriptor> shaderDescriptors;
        std::vector<PipelineDescriptor> pipelineDescriptors;
    };

    struct CommandBufferDescriptor {

    };

    struct BufferInfo {
        VkDeviceSize size;
        VkBufferUsageFlags bufferFlags;
        VmaMemoryUsage memoryFlags;
    };

    struct ImageInfo {
        VkImageType type;
        VkExtent3D extent;
        VkImageUsageFlags imageFlags;
        VmaMemoryUsage memoryFlags;
    };

    struct DrawDescriptor {
        VkBuffer vertexBuffer{nullptr};
        std::optional<VkBuffer> indexBuffer;
        size_t count{0};
        const void* pushConstant{nullptr};
    };

    struct GenericData {
        const void* data;
        size_t size;
    };
}

#endif //PIPERINE_INFO_TYPES_H
