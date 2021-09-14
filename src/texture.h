//
// Created by jack on 7/7/21.
//

#ifndef PIPERINE_TEXTURE_H
#define PIPERINE_TEXTURE_H

#include "vulkan_containers.h"

#include <stb_image.h>
#include <filesystem>

namespace pip {
    void createTexture(const std::filesystem::path& path, const Device& device, const Pipeline& pipeline);
}

#endif //PIPERINE_TEXTURE_H
