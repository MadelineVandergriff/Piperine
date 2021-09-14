//
// Created by jack on 8/22/21.
//

#include <piperine/allocators.h>

using namespace pip;
using namespace pip::detail;

AllocatorRef detail::createAllocator(const InstanceRef &instance, VkPhysicalDevice physical, const DeviceRef &device) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    allocatorInfo.physicalDevice = physical;
    allocatorInfo.device = *device;
    allocatorInfo.instance = *instance;

    AllocatorRef allocator;
    allocator.create(allocatorInfo);
    return allocator;
}

CommandPoolRef detail::createCommandPool(const DeviceRef &device, const Queues &queues) {
    VkCommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = queues.graphicsIndex;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    CommandPoolRef pool;
    pool.create(device, poolCreateInfo);
    return pool;
}
