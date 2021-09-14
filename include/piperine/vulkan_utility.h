//
// Created by jack on 7/28/21.
//

#ifndef PIPERINE_VULKAN_UTILITY_H
#define PIPERINE_VULKAN_UTILITY_H

#include <piperine/definitions.h>
#include <piperine/vulkan_types.h>
#include <vulkan/vulkan.hpp>

#include <optional>

namespace pip {
    struct SwapchainSupportInfo {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
        bool includes8BitSRGBNonlinear{false};
        bool includesMailbox{false};
    };

    struct QueueFamilies {
        std::optional<uint32_t> graphics;
        std::optional<uint32_t> present;
    };

    struct Queues {
        VkQueue graphics{nullptr};
        uint32_t graphicsIndex{0};
        VkQueue present{nullptr};
        uint32_t presentIndex{0};
    };

    struct SwapchainData {
        std::vector<VkImage> images;
        std::vector<detail::ImageViewRef> views;
        VkFormat format;
        VkExtent2D extent;
    };

    namespace detail {
        struct GenericInternals {
            InstanceRef instance;
            DebugUtilsMessengerRef messenger;
            WindowRef window;
            SurfaceRef surface;
            VkPhysicalDevice physical{nullptr};
            Queues queues;
            DeviceRef device;
            AllocatorRef allocator;
            CommandPoolRef cmdPool;
            SwapchainRef swapchain;
            SwapchainData swapchainData;
        };
    }
}

#endif //PIPERINE_VULKAN_UTILITY_H
