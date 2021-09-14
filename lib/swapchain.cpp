//
// Created by jack on 8/22/21.
//

#include <piperine/swapchain.h>
#include <piperine/device.h>

using namespace pip;
using namespace pip::detail;

std::pair<SwapchainRef, SwapchainData> detail::createSwapchain(const WindowRef& window,
                                                               const SurfaceRef& surface,
                                                               VkPhysicalDevice physical,
                                                               const DeviceRef& device,
                                                               const Queues& queues) {
    SwapchainRef swapchain;
    SwapchainData data;

    auto supportInfo = getSwapchainSupportInfo(physical, *surface);
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

    data.format = format.format;

    if (supportInfo.capabilities.currentExtent.width != UINT32_MAX) {
        data.extent = supportInfo.capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetWindowSize(*window, &width, &height);
        VkExtent2D size{(uint32_t) width, (uint32_t) height};

        data.extent.width = std::clamp(size.width,
                                            supportInfo.capabilities.minImageExtent.width,
                                            supportInfo.capabilities.maxImageExtent.width
        );

        data.extent.height = std::clamp(size.height,
                                             supportInfo.capabilities.minImageExtent.height,
                                             supportInfo.capabilities.maxImageExtent.height
        );
    }

    uint32_t imageCount = supportInfo.capabilities.maxImageCount == 0 ?
                          supportInfo.capabilities.minImageCount + 1 :
                          std::min(supportInfo.capabilities.minImageCount + 1, supportInfo.capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = *surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = format.format;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = data.extent;

    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const uint32_t indices[] = {queues.graphicsIndex, queues.presentIndex};
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

    swapchain.create(device, createInfo);

    uint32_t actualImgCount;
    vkGetSwapchainImagesKHR(*device, *swapchain, &actualImgCount, nullptr);
    data.images.resize(actualImgCount);
    data.views.resize(actualImgCount);
    vkGetSwapchainImagesKHR(*device, *swapchain, &actualImgCount, data.images.data());

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

    imageViewCreateInfo.format = data.format;
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
        imageViewCreateInfo.image = data.images.at(i);
        data.views.at(i).create(device, imageViewCreateInfo);
    }

    return {std::move(swapchain), std::move(data)};
}