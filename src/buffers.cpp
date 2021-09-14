//
// Created by jack on 6/28/21.
//

#include "buffers.h"

using namespace pip;

VmaBufferRef pip::createBuffer(const AllocatorRef& allocator, const BufferInfo &info) {
    VmaBufferRef buffer;

    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = info.size;
    bufferCreateInfo.usage = info.bufferFlags;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = info.memoryFlags;

    buffer.create(allocator, bufferCreateInfo, allocInfo);
    return buffer;
}

void pip::copyBuffer(const Device& device, const CommandPoolRef& pool, const VmaBufferRef &src, const VmaBufferRef &dst) {
    assert(src->size == dst->size);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = *pool;
    allocInfo.commandBufferCount = 1;

    CommandBufferRef cmdBuffer;
    cmdBuffer.allocate(device.logical, pool, allocInfo);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(*cmdBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = src->size;
    vkCmdCopyBuffer(*cmdBuffer, src->buffer, dst->buffer, 1, &copyRegion);

    vkEndCommandBuffer(*cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    VkCommandBuffer raw = *cmdBuffer;
    submitInfo.pCommandBuffers = &raw;

    vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.graphicsQueue);
}

void pip::populateBuffer(const AllocatorRef &allocator, VmaBufferRef &buffer, const GenericData &data) {
    assert(data.size == buffer->size);
    void* hostMemory;
    vmaMapMemory(*allocator, buffer->allocation, &hostMemory);
    memcpy(hostMemory, data.data, (size_t) buffer->size);
    vmaUnmapMemory(*allocator, buffer->allocation);
}

VmaImageRef createImage(const AllocatorRef &allocator, const ImageInfo &info) {
    VmaImageRef image;

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = info.type;
    imageInfo.extent = info.extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = info.imageFlags;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = info.memoryFlags;

    image.create(allocator, imageInfo, allocInfo);
    return image;
}
