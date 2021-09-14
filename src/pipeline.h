//
// Created by jack on 6/22/21.
//

#ifndef PIPERINE_PIPELINE_H
#define PIPERINE_PIPELINE_H

#include "vulkan_containers.h"

namespace pip {
    Pipeline createPipeline(Device& device, Swapchain& swapchain, const PipelineGraph& graph);

    namespace detail {
        Shader createShader(const DeviceRef& device, const ShaderDescriptor& descriptor);
        RenderPassRef createRenderPass(const DeviceRef& device, const Swapchain& swapchain, const RenderPassDescriptor& descriptor);
        VkPipelineShaderStageCreateInfo getShaderStageCreateInfo(const Shader& shader);
        CommandBufferVector allocateCommandBuffers(const DeviceRef& device, const CommandPoolRef& pool, uint32_t numBuffers);
    }
}

#endif //PIPERINE_PIPELINE_H
