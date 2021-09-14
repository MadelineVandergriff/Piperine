//
// Created by jack on 9/2/21.
//

#ifndef PIPERINE_VULKAN_TYPES_FORWARD_DECLARATIONS_H
#define PIPERINE_VULKAN_TYPES_FORWARD_DECLARATIONS_H

#include "piperine/public_utility.h"

namespace pip::detail {
    struct Instance;
    using InstanceRef_ = PtrRef<Instance>;

    struct Pipeline;
    using PipelineRef_ = PtrRef<Pipeline>;

    struct GLFWSingleton;
    struct InstanceRef;
    struct WindowRef;
    struct SurfaceRef;
    struct DeviceRef;
    struct SwapchainRef;
    struct ImageViewRef;
    struct ShaderModuleRef;
    struct PipelineLayoutRef;
    struct PipelineRef;
    struct RenderPassRef;
    struct FramebufferRef;
    struct CommandPoolRef;
    struct DebugUtilsMessengerRef;
    struct SemaphoreRef;
    struct FenceRef;
    struct BufferRef;
    struct DeviceMemoryRef;
    struct CommandBufferRef;
    struct CommandBufferVector;
    struct AllocatorRef;
    struct VmaBufferRef;
    struct DescriptorSetLayoutRef;
    struct DescriptorPoolRef;
    struct DescriptorSetRef;
    struct DescriptorSetVector;
    struct VmaImageRef;
}

#endif //PIPERINE_VULKAN_TYPES_FORWARD_DECLARATIONS_H
