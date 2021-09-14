//
// Created by jack on 6/17/21.
//

#ifndef PIPERINE_VULKAN_CONTAINERS_H
#define PIPERINE_VULKAN_CONTAINERS_H

#include "vulkan_types.h"
#include "info_types.h"

#include <glm/glm.hpp>
#include <filesystem>
#include <map>

namespace pip {
    struct Instance {
        InstanceRef instance;
        DebugUtilsMessengerRef debugMessenger;
    };

    struct Window {
        WindowRef window;
        SurfaceRef surface;

        uint32_t width{0};
        uint32_t height{0};
    };

    struct Device {
        VkPhysicalDevice physical{nullptr};
        DeviceRef logical;
        AllocatorRef allocator;
        CommandPoolRef pool;

        VkQueue graphicsQueue{nullptr};
        uint32_t graphicsQueueIndex{0};
        VkQueue presentQueue{nullptr};
        uint32_t presentQueueIndex{0};
    };

    struct Swapchain {
        SwapchainRef swapchain;
        std::vector<VkImage> images;
        std::vector<ImageViewRef> views;

        VkFormat format;
        VkExtent2D extent;
    };

    struct Shader {
        ShaderModuleRef module;
        ShaderType type{ShaderType::UNKNOWN};

        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<VkVertexInputBindingDescription> bindings;
    };

    struct Pipeline {
        PipelineLayoutRef layout;
        PipelineRef pipeline;
        PipelineGraph graph;
        RenderPassRef renderPass;
        DescriptorSetLayoutRef descriptorSetLayout;
        DescriptorPoolRef descriptorPool;
        DescriptorSetVector descriptorSets;
        std::vector<FramebufferRef> framebuffers;
        std::vector<VmaBufferRef> uniformBuffers;
        CommandBufferVector commandBuffers;
        std::map<PIPERINE_KEY_TYPE, Shader> shaders;
        VkExtent2D extent;
    };

    struct Mesh {
        const VmaBufferRef vertices{};
        const std::optional<VmaBufferRef> indices{};
        const size_t vertexCount{0};
        const size_t indexCount{0};
    };

    template<typename Push>
    struct Model {
        Mesh mesh;
        Push push;
        Ticket ticket;
    };

    struct CommandBuffer {
        CommandBufferRef buffer;
        PipelineLayoutRef layout;
        VkQueue queue{nullptr};
    };
}

#endif //PIPERINE_VULKAN_CONTAINERS_H
