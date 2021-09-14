//
// Created by jack on 6/16/21.
//

#ifndef PIPERINE_VULKAN_TYPES_H
#define PIPERINE_VULKAN_TYPES_H

#include "utility.h"

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <GLFW/glfw3.h>

namespace pip {
    struct InstanceRef : PtrRef<VkInstance> {
        void create(VkInstanceCreateInfo info);
    };

    struct WindowRef : PtrRef<GLFWwindow*> {
        void create(WindowInfo info);
    };

    struct SurfaceRef : PtrRef<VkSurfaceKHR> {
        void create(InstanceRef instance, const WindowRef& window);
    };

    struct DeviceRef : PtrRef<VkDevice> {
        void create(VkPhysicalDevice phys, VkDeviceCreateInfo info);
    };

    struct SwapchainRef : PtrRef<VkSwapchainKHR> {
        void create(DeviceRef device, VkSwapchainCreateInfoKHR info);
    };

    struct ImageViewRef : PtrRef<VkImageView> {
        void create(DeviceRef device, VkImageViewCreateInfo info);
    };

    struct ShaderModuleRef : PtrRef<VkShaderModule> {
        void create(DeviceRef device, VkShaderModuleCreateInfo info);
    };

    struct PipelineLayoutRef : PtrRef<VkPipelineLayout> {
        void create(DeviceRef device, VkPipelineLayoutCreateInfo info);
    };

    struct PipelineRef : PtrRef<VkPipeline> {
        void create(DeviceRef device, VkGraphicsPipelineCreateInfo info);
    };

    struct RenderPassRef : PtrRef<VkRenderPass> {
        void create(DeviceRef device, VkRenderPassCreateInfo info);
    };

    struct FramebufferRef : PtrRef<VkFramebuffer> {
        void create(DeviceRef device, VkFramebufferCreateInfo info);
    };

    struct CommandPoolRef : PtrRef<VkCommandPool> {
        void create(DeviceRef device, VkCommandPoolCreateInfo info);
    };

    struct DebugUtilsMessengerRef : PtrRef<VkDebugUtilsMessengerEXT> {
        void create(InstanceRef instance, VkDebugUtilsMessengerCreateInfoEXT info);
    };

    struct SemaphoreRef : PtrRef<VkSemaphore> {
        void create(DeviceRef device, VkSemaphoreCreateInfo info);
    };

    struct FenceRef : PtrRef<VkFence> {
        void create(DeviceRef device, VkFenceCreateInfo info);
    };

    struct BufferRef : PtrRef<VkBuffer> {
        void create(DeviceRef device, VkBufferCreateInfo info);
    };

    struct DeviceMemoryRef : PtrRef<VkDeviceMemory> {
        void allocate(DeviceRef device, VkMemoryAllocateInfo info);
    };

    struct CommandBufferRef : PtrRef<VkCommandBuffer> {
        void allocate(DeviceRef device, CommandPoolRef pool, VkCommandBufferAllocateInfo info);
    };

    struct CommandBufferVector : Ref<std::vector<VkCommandBuffer>> {
        void allocate(DeviceRef device, CommandPoolRef pool, VkCommandBufferAllocateInfo info);
    };

    struct AllocatorRef : PtrRef<VmaAllocator> {
        void create(VmaAllocatorCreateInfo info);
    };

    struct VmaBuffer {
        VkBuffer buffer{nullptr};
        VmaAllocation allocation{nullptr};
        VkDeviceSize size{0};
    };

    struct VmaBufferRef : Ref<VmaBuffer> {
        void create(AllocatorRef allocator, VkBufferCreateInfo bufferInfo, VmaAllocationCreateInfo allocInfo);
    };

    struct DescriptorSetLayoutRef : PtrRef<VkDescriptorSetLayout> {
        void create(DeviceRef device, VkDescriptorSetLayoutCreateInfo info);
    };

    struct DescriptorPoolRef : PtrRef<VkDescriptorPool> {
        void create(DeviceRef device, VkDescriptorPoolCreateInfo info);
    };

    struct DescriptorSet : PtrRef<VkDescriptorSet> {
        void allocate(DeviceRef device, DescriptorPoolRef pool, VkDescriptorSetAllocateInfo info);
    };

    struct DescriptorSetVector : Ref<std::vector<VkDescriptorSet>> {
        void allocate(DeviceRef device, DescriptorPoolRef pool, VkDescriptorSetAllocateInfo info);
    };

    struct VmaImage {
        VkImage image{nullptr};
        VmaAllocation allocation{nullptr};
    };

    struct VmaImageRef : Ref<VmaImage> {
        void create(AllocatorRef allocator, VkImageCreateInfo imageInfo, VmaAllocationCreateInfo allocInfo);
    };
}

#endif //PIPERINE_VULKAN_TYPES_H
