//
// Created by jack on 6/17/21.
//

#ifndef PIPERINE_WINDOW_H
#define PIPERINE_WINDOW_H

#include "vulkan_containers.h"

namespace pip {
    std::vector<const char*> getInstanceExtensions();
    std::vector<const char*> getValidationLayers();
    std::vector<const char*> getDeviceExtensions();
    Instance InitializeVulkan(const char* title);
    Window createWindow(InstanceRef& instance, WindowInfo info);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
    );
}

#endif //PIPERINE_WINDOW_H
