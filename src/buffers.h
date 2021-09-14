//
// Created by jack on 6/28/21.
//

#ifndef PIPERINE_BUFFERS_H
#define PIPERINE_BUFFERS_H

#include "vulkan_containers.h"

#include <vk_mem_alloc.h>

namespace pip {
    VmaBufferRef createBuffer(const AllocatorRef& allocator, const BufferInfo& info);
    VmaImageRef createImage(const AllocatorRef& allocator, const ImageInfo& info);

    template<typename Vec>
    void populateBuffer(const AllocatorRef& allocator, VmaBufferRef& buffer, const Vec& data) {
        void* hostMemory;
        vmaMapMemory(*allocator, buffer->allocation, &hostMemory);
        memcpy(hostMemory, data.data(), (size_t) buffer->size);
        vmaUnmapMemory(*allocator, buffer->allocation);
    }

    void populateBuffer(const AllocatorRef& allocator, VmaBufferRef& buffer, const GenericData& data);

    void copyBuffer(const Device& device, const CommandPoolRef& pool, const VmaBufferRef& src, const VmaBufferRef& dst);
}

#endif //PIPERINE_BUFFERS_H
