//
// Created by jack on 8/22/21.
//

#include <piperine/messenger.h>

using namespace pip;
using namespace pip::detail;

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

constexpr VkDebugUtilsMessengerCreateInfoEXT detail::getMessengerCreateInfo() {
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
    return messengerInfo;
}

DebugUtilsMessengerRef detail::createMessenger(const InstanceRef &instance) {
    DebugUtilsMessengerRef messenger;
#ifdef PIPERINE_ENABLE_VALIDATION
    messenger.create(instance, getMessengerCreateInfo());
#endif
    return messenger;
}
