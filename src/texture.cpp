//
// Created by jack on 7/7/21.
//

#include "texture.h"
#include "buffers.h"

using namespace pip;

void pip::createTexture(const std::filesystem::path &path, const Device &device, const Pipeline &pipeline) {
    /*int width, height, channels;
    stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;
    assert(pixels != nullptr);

    auto stagingBuffer = createBuffer(device.allocator, BufferInfo{
        .size = imageSize,
        .bufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .memoryFlags = VMA_MEMORY_USAGE_CPU_ONLY
    });

    populateBuffer(device.allocator, stagingBuffer, GenericData{pixels, imageSize});
    stbi_image_free(pixels);

    auto image = createImage(device.allocator, ImageInfo{
       .type = VK_IMAGE_TYPE_2D,
       .extent = {(uint32_t) width, (uint32_t) height, 1},
       .imageFlags = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
       .memoryFlags = VMA_MEMORY_USAGE_GPU_ONLY
    });*/
}
