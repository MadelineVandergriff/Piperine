//
// Created by jack on 6/17/21.
//

#include "window.h"

using namespace pip;

Instance pip::InitializeVulkan(const char* title) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = title;
    appInfo.pEngineName = "Piperine";
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.applicationVersion = VK_API_VERSION_1_0;
    appInfo.engineVersion = VK_API_VERSION_1_0;

    glfwInit();
    auto extensions = getInstanceExtensions();

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    #ifdef PIPERINE_ENABLE_VALIDATION
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo{};
    messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messengerInfo.pfnUserCallback = debugCallback;
    messengerInfo.pUserData = nullptr;

    auto validationLayers = getValidationLayers();
    instanceInfo.pNext = &messengerInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    #else
    instanceInfo.pNext = nullptr;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    #endif

    InstanceRef instance;
    instance.create(instanceInfo);

    DebugUtilsMessengerRef messenger;
    #ifdef PIPERINE_ENABLE_VALIDATION
    messenger.create(instance, messengerInfo);
    #endif

    return {instance, messenger};
}

VkBool32 pip::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                       void *pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        log(pCallbackData->pMessage, LogType::VALIDATION, LogSeverity::ERROR);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        log(pCallbackData->pMessage, LogType::VALIDATION, LogSeverity::WARNING);
    } else {
        log(pCallbackData->pMessage, LogType::VALIDATION, LogSeverity::LOG);
    }
    return VK_FALSE;
}

std::vector<const char *> pip::getValidationLayers() {
    return {"VK_LAYER_KHRONOS_validation"};
}

std::vector<const char *> pip::getInstanceExtensions() {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    #ifdef PIPERINE_ENABLE_VALIDATION
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    return extensions;
}

Window pip::createWindow(InstanceRef &instance, WindowInfo info) {
    Window window;
    window.window.create(info);
    window.surface.create(instance, window.window);
    int w, h;
    glfwGetFramebufferSize(*window.window, &w, &h);
    window.width = w;
    window.height = h;
    return window;
}

std::vector<const char *> pip::getDeviceExtensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

