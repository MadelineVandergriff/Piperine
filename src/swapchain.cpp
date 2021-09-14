//
// Created by jack on 6/22/21.
//

#include "swapchain.h"

using namespace pip;

Swapchain pip::createSwapchain(Device &device, Window &window) {
    Swapchain swapchain;
    auto supportInfo = getSwapchainSupportInfo(device.physical, *window.surface);

    VkPresentModeKHR presentMode;
    if (supportInfo.includesMailbox) {
        presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    } else {
        presentMode = supportInfo.presentModes.front();
    }

    VkSurfaceFormatKHR format;
    if (supportInfo.includes8BitSRGBNonlinear) {
        format.format = VK_FORMAT_B8G8R8A8_SRGB;
        format.colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    } else {
        format = supportInfo.formats.front();
    }

    swapchain.format = format.format;

    if (supportInfo.capabilities.currentExtent.width != UINT32_MAX) {
        swapchain.extent = supportInfo.capabilities.currentExtent;
    } else {
        VkExtent2D size{window.width, window.height};

        swapchain.extent.width = std::clamp(size.width,
            supportInfo.capabilities.minImageExtent.width,
            supportInfo.capabilities.maxImageExtent.width
        );

        swapchain.extent.height = std::clamp(size.height,
            supportInfo.capabilities.minImageExtent.height,
            supportInfo.capabilities.maxImageExtent.height
        );
    }

    uint32_t imageCount = supportInfo.capabilities.maxImageCount == 0 ?
                          supportInfo.capabilities.minImageCount + 1 :
                          std::min(supportInfo.capabilities.minImageCount + 1, supportInfo.capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *window.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = swapchain.extent;

    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t indices[] = {device.graphicsQueueIndex, device.presentQueueIndex};
    if (indices[0] == indices[1]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = indices;
    }

    createInfo.preTransform = supportInfo.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    swapchain.swapchain.create(device.logical, createInfo);

    uint32_t actualImgCount;
    vkGetSwapchainImagesKHR(*device.logical, *swapchain.swapchain, &actualImgCount, nullptr);
    swapchain.images.resize(actualImgCount);
    swapchain.views.resize(actualImgCount);
    vkGetSwapchainImagesKHR(*device.logical, *swapchain.swapchain, &actualImgCount, swapchain.images.data());

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.format = swapchain.format;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    for (size_t i = 0; i < actualImgCount; i++) {
        imageViewCreateInfo.image = swapchain.images.at(i);
        swapchain.views.at(i).create(device.logical, imageViewCreateInfo);
    }

    return swapchain;
}

SwapchainSupportInfo pip::getSwapchainSupportInfo(VkPhysicalDevice physical, VkSurfaceKHR surface) {
    SwapchainSupportInfo info;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, surface, &info.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount, nullptr);
    if (formatCount > 0) {
        info.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical, surface, &formatCount, info.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &presentModeCount, nullptr);
    if (presentModeCount > 0) {
        info.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical, surface, &presentModeCount, info.presentModes.data());
    }

    for (const auto& format : info.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            info.includes8BitSRGBNonlinear = true;
        }
    }

    for (const auto& mode : info.presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            info.includesMailbox = true;
        }
    }

    return info;
}
