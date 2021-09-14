//
// Created by jack on 6/21/21.
//

#ifndef PIPERINE_DEVICE_H
#define PIPERINE_DEVICE_H

#include "vulkan_containers.h"

#include <optional>

namespace pip {
    Device createDevice(const InstanceRef& instance, const SurfaceRef& surface);

    struct QueueProperties {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isViable() const;
    };

    namespace detail {
        uint32_t rankDevice(VkPhysicalDevice physical, VkSurfaceKHR surface);
        QueueProperties getQueueProperties(VkPhysicalDevice physical, VkSurfaceKHR surface);

        static constexpr uint32_t combinedQueueRank = 10;
        static constexpr uint32_t dedicatedGraphicsRank = 100;
    }
}

#endif //PIPERINE_DEVICE_H
