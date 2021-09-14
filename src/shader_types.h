//
// Created by jack on 6/24/21.
//

#ifndef PIPERINE_SHADER_TYPES_H
#define PIPERINE_SHADER_TYPES_H

#include "attribute_generator.h"
#include "vulkan_containers.h"

namespace pip {
    struct Vertex : AttributeGenerator<glm::vec3, glm::vec3> {
        glm::vec3 pos;
        glm::vec3 color;
    };

    struct MVPConstant {
        glm::mat4 mvp;
    };

    template<typename T>
    ShaderBinding makeBinding(uint32_t index) {
        ShaderBinding shaderBinding{};

        shaderBinding.binding.binding = index;
        shaderBinding.binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        shaderBinding.binding.stride = sizeof(T);

        auto attributeArray = T::generateAttributeDescriptions(index);
        std::copy(std::begin(attributeArray), std::end(attributeArray), std::back_inserter(shaderBinding.attributes));

        return shaderBinding;
    }
}

#endif //PIPERINE_SHADER_TYPES_H
