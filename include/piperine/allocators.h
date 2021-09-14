//
// Created by jack on 8/22/21.
//

#ifndef PIPERINE_ALLOCATORS_H
#define PIPERINE_ALLOCATORS_H

#include <piperine/interface.h>
#include <piperine/vulkan_types.h>
#include <piperine/vulkan_utility.h>

namespace pip::detail {
    AllocatorRef createAllocator(const InstanceRef& instance, VkPhysicalDevice physical, const DeviceRef& device);
    CommandPoolRef createCommandPool(const DeviceRef& device, const Queues& queues);
}

#endif //PIPERINE_ALLOCATORS_H
