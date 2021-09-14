//
// Created by jack on 8/22/21.
//

#ifndef PIPERINE_SWAPCHAIN_H
#define PIPERINE_SWAPCHAIN_H

#include <piperine/vulkan_types.h>
#include <piperine/vulkan_utility.h>

#include <tuple>

namespace pip::detail {
    std::pair<SwapchainRef, SwapchainData> createSwapchain(const WindowRef& window,
                                                           const SurfaceRef& surface,
                                                           VkPhysicalDevice physical,
                                                           const DeviceRef& device,
                                                           const Queues& queues);
}

#endif //PIPERINE_SWAPCHAIN_H
