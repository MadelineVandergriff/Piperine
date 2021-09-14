//
// Created by jack on 8/22/21.
//

#ifndef PIPERINE_MESSENGER_H
#define PIPERINE_MESSENGER_H

#include <piperine/vulkan_types.h>

namespace pip::detail {
    DebugUtilsMessengerRef createMessenger(const InstanceRef& instance);
    constexpr VkDebugUtilsMessengerCreateInfoEXT getMessengerCreateInfo();
}

namespace pip {
    VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                           void *pUserData);
}

#endif //PIPERINE_MESSENGER_H
