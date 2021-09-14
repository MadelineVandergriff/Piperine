//
// Created by jack on 7/26/21.
//

#ifndef PIPERINE_INTERFACE_H
#define PIPERINE_INTERFACE_H

#include "piperine/definitions.h"
#include "piperine/synchronization.h"
#include "piperine/public_utility.h"
#include "piperine/vulkan_types_forward_declarations.h"

#include <type_traits>
#include <vector>
#include <map>

namespace pip {
    enum class PIPERINE_API ShaderTypes {
        VERTEX, FRAGMENT, GEOMETRY, TESSELATION, COMPUTE, UNKNOWN
    };

    using ShortString = std::array<char, 16>;
    using MediumString = std::array<char, 32>;
    using LongString = std::array<char, 64>;

    namespace WindowFlags {
        enum PIPERINE_API : PIPERINE_ENUM {
            FULLSCREEN = 1,
            RESIZABLE = 1 << 1
        };
    }

    struct PIPERINE_API ShaderCreateInfo {
        ShaderTypes type = ShaderTypes::UNKNOWN;
        PIPERINE_SHORT_STRING id{'\0'};
        uint32_t pushConstantSize{0};
        uint32_t uniformSize{0};
    };

    struct PIPERINE_API ShaderStageDescriptors {
        ShaderCreateInfo vertex;
        ShaderCreateInfo fragment;
        ShaderCreateInfo geometry;
        ShaderCreateInfo tesselation;
        ShaderCreateInfo compute;
    };

    struct PIPERINE_API UniformDescriptor {

    };

    struct PIPERINE_API PipelineDescriptor {
        PIPERINE_SHORT_STRING id = "";
        UniformDescriptor uniforms;
        ShaderStageDescriptors stages;
    };

    struct PIPERINE_API WindowCreateInfo {
        uint16_t width{800};
        uint16_t height{600};
        PIPERINE_ENUM flags{0};
        PIPERINE_LONG_STRING title{"Piperine Application"};
        uint32_t version{1u << 22};
    };

    template<size_t NumPipelines, size_t NumBuffers>
    struct PIPERINE_API InterfaceCreateInfo {
        WindowCreateInfo windowInfo;
        std::array<PipelineDescriptor, NumPipelines> pipelines;
    };

    namespace detail {
        struct GenericInternals;
        using GenericInternalsPtr = std::unique_ptr<GenericInternals, void(*)(GenericInternals*)>;

        struct GenericInternalsCreateInfo {
            WindowCreateInfo windowInfo;
        };

        struct ExtensionLayerRetrievalInfo {

        };

        GenericInternalsPtr genericInitialization(const GenericInternalsCreateInfo& info);
        void genericInternalsDeleter(GenericInternals*);

        struct ExtensionLayerInfo {
            std::vector<const char*> instanceExtensions;
            std::vector<const char*> deviceExtensions;
            std::vector<const char*> validationLayers;
        };

        ExtensionLayerInfo getExtensionLayerInfo(const ExtensionLayerRetrievalInfo& info);

        template<bool B, typename T> struct Optional;
        template<typename T> struct Optional<true, T> : T {};
        template<typename T> struct Optional<false, T> {};

        template<uint32_t pushConstantSize>
        struct PushConstantFeature {
            RingBufferWriter<int, 8, false> pushConstantWriter;
        };

        template<ShaderCreateInfo info>
        struct ShaderFeature :
            Optional<(info.pushConstantSize > 0), PushConstantFeature<info.pushConstantSize>> {};

        template<size_t size, std::array<PipelineDescriptor, size> descriptors>
        struct PipelineFeatures {
            std::map<ShortString, PipelineRef*> pipelineMap;
            std::array<PipelineRef_, size> pipelines;
        };

        void populatePipelineFeatures(std::vector<PipelineDescriptor> descriptors);
    }

    template<InterfaceCreateInfo info>
    struct PIPERINE_API Interface :
            detail::PipelineFeatures<info.pipelines.size(), info.pipelines> {
        detail::GenericInternalsPtr genericInternals{nullptr, detail::genericInternalsDeleter};
        detail::erased_ptr internal{nullptr, detail::null_deleter};
    };

    template<InterfaceCreateInfo info>
    Interface<info> makeInterface() {
        Interface<info> interface;
        interface.genericInternals = detail::genericInitialization({info.windowInfo});
        return interface;
    }
}

#endif //PIPERINE_INTERFACE_H
