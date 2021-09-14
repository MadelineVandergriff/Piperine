//
// Created by jack on 8/25/21.
//

#ifndef PIPERINE_INTERFACE_BUILDER_H
#define PIPERINE_INTERFACE_BUILDER_H

#include <piperine/interface.h>

namespace pip {
    namespace detail {
        template<typename T, T t>
        struct ConstexprValueWrapper {

        };
    }

    template<InterfaceCreateInfo info>
    class InterfaceBuilder {
    public:
        constexpr auto build() {
            static_assert(info.pipelines.size() > 0, "There must be at least 1 pipeline");
            static_assert(checkPipelines<info>(), "All pipelines must have a vertex and fragment shader OR a compute shader");
            //return info;
            return makeInterface<info>();
        }

        template<ShortString id>
        constexpr auto addPipeline() {
            return InterfaceBuilder<addPipeline<info, id>()>{};
        }

        template<WindowCreateInfo windowInfo>
        constexpr auto setProperty() {
            return InterfaceBuilder<merge<info, windowInfo>()>{};
        }

        template<ShaderCreateInfo shaderInfo>
        constexpr auto setProperty() {
            return InterfaceBuilder<merge<info, shaderInfo, shaderInfo.type>()>{};
        }
    private:
        template<InterfaceCreateInfo _info>
        static constexpr bool checkPipelines() {
            for (const auto& pipeline : _info.pipelines) {
                if (!((pipeline.stages.fragment.type == ShaderTypes::UNKNOWN
                || pipeline.stages.vertex.type == ShaderTypes::UNKNOWN)
                ^ (pipeline.stages.compute.type == ShaderTypes::UNKNOWN))) return false;
            }
            return true;
        }

        template<size_t N>
        static constexpr std::array<char, N> getCharArray(const char (&chars)[N]) {
            std::array<char, N> array;
            for (size_t i = 0; i < N; i++) {
                array[i] = chars[i];
            }
        }

        template<size_t P, size_t B>
        struct InterfaceCreateInfoValues {
            static constexpr size_t NumPipelines = P;
            static constexpr size_t NumBuffers = B;
        };

        template<size_t P, size_t B>
        constexpr static auto getValues(InterfaceCreateInfo<P, B>) {
            return InterfaceCreateInfoValues<P, B>{};
        }

        template<InterfaceCreateInfo _info>
        using Values = decltype(getValues(_info));

        template<InterfaceCreateInfo _info, WindowCreateInfo _windowInfo>
        constexpr static auto merge() {
            auto copy = _info;
            copy.windowInfo = _windowInfo;
            return copy;
        }

        template<InterfaceCreateInfo _info, ShaderCreateInfo _shaderInfo, ShaderTypes _type>
        constexpr static auto merge() {
            static_assert(_info.pipelines.size() > 0, "There must be at least 1 pipeline to set shaders");
            static_assert(_shaderInfo.type != ShaderTypes::UNKNOWN, "Shader type must be specified");
            auto copy = _info;
            auto &stages = copy.pipelines.back().stages;
            switch (_type) {
                case ShaderTypes::VERTEX:
                    stages.vertex = _shaderInfo;
                    break;
                case ShaderTypes::FRAGMENT:
                    stages.fragment = _shaderInfo;
                    break;
                case ShaderTypes::GEOMETRY:
                    stages.geometry = _shaderInfo;
                    break;
                case ShaderTypes::TESSELATION:
                    stages.tesselation = _shaderInfo;
                    break;
                case ShaderTypes::COMPUTE:
                    stages.compute = _shaderInfo;
                    break;
            }
            return copy;
        }

        template<InterfaceCreateInfo _info, ShortString id>
        constexpr static auto addPipeline() {
            InterfaceCreateInfo<Values<_info>::NumPipelines + 1, Values<_info>::NumBuffers> copy;
            copy.windowInfo = _info.windowInfo;
            std::copy(_info.pipelines.begin(), _info.pipelines.end(), copy.pipelines.begin());
            std::copy(id.begin(), id.end(), std::begin(copy.pipelines.back().id));
            return copy;
        }
    };

    using BasicBuilder = InterfaceBuilder<InterfaceCreateInfo<0, 0>{}>;
}

#endif //PIPERINE_INTERFACE_BUILDER_H
