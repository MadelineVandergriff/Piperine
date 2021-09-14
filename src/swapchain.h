//
// Created by jack on 6/22/21.
//

#ifndef PIPERINE_SWAPCHAIN_H
#define PIPERINE_SWAPCHAIN_H

#include "vulkan_containers.h"

namespace pip {
    Swapchain createSwapchain(Device& device, Window& window);
    SwapchainSupportInfo getSwapchainSupportInfo(VkPhysicalDevice physical, VkSurfaceKHR surface);
}

#endif //PIPERINE_SWAPCHAIN_H
