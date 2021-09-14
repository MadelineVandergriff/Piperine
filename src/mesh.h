//
// Created by jack on 7/1/21.
//

#ifndef PIPERINE_MESH_H
#define PIPERINE_MESH_H

#include "vulkan_containers.h"
#include "buffers.h"

namespace pip {
    template<typename Vec>
    Mesh createMesh(Device& device, const Vec& vertices) {
        Mesh mesh{createBuffer(device.allocator, {.size = sizeof(vertices.at(0)) * vertices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_GPU_ONLY}), {}, vertices.size(), 0};

        auto stagingBuffer = createBuffer(device.allocator, {.size = sizeof(vertices.at(0)) * vertices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_CPU_ONLY});

        populateBuffer(device.allocator, stagingBuffer, vertices);
        copyBuffer(device, device.pool, stagingBuffer, mesh.vertices);

        return mesh;
    }

    template<typename Vec>
    Mesh createMesh(Device& device, const Vec& vertices, const std::vector<uint16_t>& indices) {
        Mesh mesh{
            createBuffer(device.allocator, {.size = sizeof(vertices.at(0)) * vertices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_GPU_ONLY}
            ),
            createBuffer(device.allocator, {.size = sizeof(uint16_t) * indices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_GPU_ONLY}
            ), vertices.size(), indices.size()
        };

        auto vertexStagingBuffer = createBuffer(device.allocator, {.size = sizeof(vertices.at(0)) * vertices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_CPU_ONLY});

        auto indexStagingBuffer = createBuffer(device.allocator, {.size = sizeof(uint16_t) * indices.size(),
                .bufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .memoryFlags = VMA_MEMORY_USAGE_CPU_ONLY});

        populateBuffer(device.allocator, vertexStagingBuffer, vertices);
        populateBuffer(device.allocator, indexStagingBuffer, indices);
        copyBuffer(device, device.pool, vertexStagingBuffer, mesh.vertices);
        copyBuffer(device, device.pool, indexStagingBuffer, mesh.indices.value());

        return mesh;
    }

    template<typename T>
    DrawDescriptor getDrawDescriptor(const Model<T>& model) {
        if (model.mesh.indices.has_value()) {
            return {model.mesh.vertices->buffer, model.mesh.indices.value()->buffer, model.mesh.indexCount, &model.push};
        } else {
            return {model.mesh.vertices->buffer, {}, model.mesh.vertexCount, &model.push};
        }
    }
}

#endif //PIPERINE_MESH_H
