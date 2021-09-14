//
// Created by jack on 6/22/21.
//

#include "commands.h"

using namespace pip;

void pip::endCommandBuffer(VkCommandBuffer buffer) {
    vkCmdEndRenderPass(buffer);
    assert(vkEndCommandBuffer(buffer) == VK_SUCCESS);
}

void pip::drawCommandBuffer(VkCommandBuffer buffer, const Pipeline& pipeline, DrawDescriptor descriptor) {
    VkBuffer vertexBuffers[] = {descriptor.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets); // TODO not hardcoded

    auto pushSize = pipeline.graph.shaderDescriptors.at(pipeline.graph.pipelineDescriptors.at(0).vertexShaderKey).pushConstantSize;
    if (pushSize.has_value()) {
        vkCmdPushConstants(
            buffer,
            *pipeline.layout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            pushSize.value(),
            descriptor.pushConstant
        );
    }

    if (descriptor.indexBuffer.has_value()) {
        vkCmdBindIndexBuffer(buffer, descriptor.indexBuffer.value(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed(buffer, descriptor.count, 1, 0, 0, 0);
    } else {
        vkCmdDraw(buffer, descriptor.count, 1, 0, 0);
    }
}

VkCommandBuffer pip::startCommandBuffer(const Pipeline& pipeline, size_t index) {
    VkCommandBuffer buffer = pipeline.commandBuffers->at(index);

    VkCommandBufferBeginInfo bufferBeginInfo{};
    bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferBeginInfo.flags = 0;
    bufferBeginInfo.pInheritanceInfo = nullptr;

    assert(vkBeginCommandBuffer(buffer, &bufferBeginInfo) == VK_SUCCESS);

    VkRenderPassBeginInfo renderPassBeginInfo{};
    VkClearValue clearValue = {0.f, 0.f, 0.f, 1.f};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.framebuffer = *pipeline.framebuffers.at(index);
    renderPassBeginInfo.renderPass = *pipeline.renderPass;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = pipeline.extent;
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(buffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *pipeline.pipeline);

    return buffer;
}