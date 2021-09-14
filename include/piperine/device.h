//
// Created by jack on 8/22/21.
//

#ifndef PIPERINE_DEVICE_H
#define PIPERINE_DEVICE_H

#include <piperine/interface.h>
#include <piperine/vulkan_types.h>
#include <piperine/vulkan_utility.h>

namespace pip::detail {
    VkPhysicalDevice getPhysical(const InstanceRef& instance, const SurfaceRef& surface, const ExtensionLayerInfo& layerInfo);
    Queues getQueues(VkPhysicalDevice physical, const DeviceRef& device, const SurfaceRef& surface);
    DeviceRef createDevice(VkPhysicalDevice physical, const SurfaceRef& surface, const ExtensionLayerInfo& layerInfo);

    uint32_t rankDevice(VkPhysicalDevice physical, VkSurfaceKHR surface, const ExtensionLayerInfo& layerInfo);
    QueueFamilies getQueueFamilies(VkPhysicalDevice physical, VkSurfaceKHR surface);
    SwapchainSupportInfo getSwapchainSupportInfo(VkPhysicalDevice physical, VkSurfaceKHR surface);
    bool isQueueViable(const QueueFamilies& families);
    static constexpr uint32_t combinedQueueRank = 10;
    static constexpr uint32_t dedicatedGraphicsRank = 100;
}

#endif //PIPERINE_DEVICE_H
