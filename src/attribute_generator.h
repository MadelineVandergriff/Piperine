//
// Created by jack on 6/27/21.
//

#ifndef PIPERINE_ATTRIBUTE_GENERATOR_H
#define PIPERINE_ATTRIBUTE_GENERATOR_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include <array>
#include <utility>

namespace pip {
    namespace detail {
        template<typename... Ts>
        struct offset_calc;
    }

    template<typename... Ts>
    class AttributeGenerator {
    public:
        static std::array<VkVertexInputAttributeDescription, sizeof...(Ts)> generateAttributeDescriptions(uint32_t binding) {
            std::array<VkVertexInputAttributeDescription, sizeof...(Ts)> descriptions{};
            VkVertexInputAttributeDescription description{};
            for (size_t i = 0; i < sizeof...(Ts); i++) {
                description.binding = binding;
                description.format = formats.at(i);
                description.offset = detail::offset_calc<Ts...>::offsets.at(i);
                description.location = i;

                descriptions.at(i) = description;
            }

            return descriptions;
        };
    private:
        template<typename T>
        static constexpr VkFormat chooseFormat() {
            if constexpr (std::is_same_v<T, glm::vec2>) {
                return VK_FORMAT_R32G32_SFLOAT;
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                return VK_FORMAT_R32G32B32_SFLOAT;
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            } else if constexpr (std::is_same_v<T, float>) {
                return VK_FORMAT_R32_SFLOAT;
            } else if constexpr (std::is_same_v<T, glm::ivec2>) {
                return VK_FORMAT_R32G32_SINT;
            } else if constexpr (std::is_same_v<T, glm::ivec3>) {
                return VK_FORMAT_R32G32B32_SINT;
            } else if constexpr (std::is_same_v<T, glm::ivec4>) {
                return VK_FORMAT_R32G32B32A32_SINT;
            } else if constexpr (std::is_same_v<T, int>) {
                return VK_FORMAT_R32_SINT;
            } else if constexpr (std::is_same_v<T, glm::dvec2>) {
                return VK_FORMAT_R64G64_SFLOAT;
            } else if constexpr (std::is_same_v<T, glm::dvec3>) {
                return VK_FORMAT_R64G64B64_SFLOAT;
            } else if constexpr (std::is_same_v<T, glm::dvec4>) {
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            } else if constexpr (std::is_same_v<T, double>) {
                return VK_FORMAT_R64_SFLOAT;
            }
        }

        static constexpr std::array<VkFormat, sizeof...(Ts)> formats{chooseFormat<Ts>()...};
    };

    namespace detail {
        template<typename, typename...>
        struct offset_impl;

        template<size_t... Offsets>
        struct offset_array {
        };

        template<typename OffsetArray, size_t Prepend>
        struct offset_array_prepend;

        template<size_t Prepend, size_t... Rest>
        struct offset_array_prepend<offset_array<Rest...>, Prepend> {
            using type = offset_array<Prepend, Rest...>;
        };

        template<typename OffsetArray>
        struct offset_array_value;

        template<size_t... Offsets>
        struct offset_array_value<offset_array<Offsets...>> {
            static constexpr std::array offsets{Offsets...};
        };

        template<typename... Ts>
        struct tuple_like;

        template<typename First, typename... Rest>
        struct tuple_like<First, Rest...> {
            First first;
            tuple_like<Rest...> rest;
        };

        template<typename Last>
        struct tuple_like<Last> {
            Last last;
        };

        template<typename... Ts>
        struct offset_calc {
            tuple_like<Ts...> tuple;
            static constexpr std::array<size_t, sizeof...(Ts)> offsets = offset_array_value<typename offset_impl<offset_calc<Ts...>, Ts...>::Offsets>::offsets;
        };

        template<typename Base, typename Last>
        struct offset_impl<Base, Last> {
            Last last;

            using Offsets = offset_array<sizeof(Base) - sizeof(Last)>;
        };

        template<typename Base, typename First, typename... Rest>
        struct offset_impl<Base, First, Rest...> {
            First first;
            offset_impl<Base, Rest...> rest;

            using Offsets = typename offset_array_prepend<typename decltype(rest)::Offsets,
                    sizeof(Base) - sizeof(First) - sizeof(rest)>::type;
        };
    }
}

#endif //PIPERINE_ATTRIBUTE_GENERATOR_H
