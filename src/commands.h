//
// Created by jack on 6/22/21.
//

#ifndef PIPERINE_COMMANDS_H
#define PIPERINE_COMMANDS_H

#include "vulkan_containers.h"

namespace pip {
    VkCommandBuffer startCommandBuffer(const Pipeline& pipeline, size_t index);
    void drawCommandBuffer(VkCommandBuffer buffer, const Pipeline& pipeline, DrawDescriptor descriptor);
    void endCommandBuffer(VkCommandBuffer buffer);

    template<typename Push>
    void pushConstantCommandBuffer(VkCommandBuffer buffer, Pipeline& pipeline, Push push) {
        auto vertexKey = pipeline.graph.pipelineDescriptors.at(0).vertexShaderKey;
        assert(pipeline.graph.shaderDescriptors.at(vertexKey).pushConstantSize == sizeof(push));

        vkCmdPushConstants(buffer, *pipeline.layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(push), &push);
    }
}

#endif //PIPERINE_COMMANDS_H
