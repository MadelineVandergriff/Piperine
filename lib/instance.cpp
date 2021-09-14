//
// Created by jack on 8/22/21.
//

#include <piperine/instance.h>
#include <piperine/messenger.h>

using namespace pip;
using namespace pip::detail;

InstanceRef detail::createInstance(const WindowCreateInfo &windowInfo, const ExtensionLayerInfo& layerInfo) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = windowInfo.title;
    appInfo.pEngineName = "Piperine";
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.applicationVersion = windowInfo.version;
    appInfo.engineVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = layerInfo.instanceExtensions.size();
    instanceInfo.ppEnabledExtensionNames = layerInfo.instanceExtensions.data();

#ifdef PIPERINE_ENABLE_VALIDATION
    VkDebugUtilsMessengerCreateInfoEXT messengerInfo{getMessengerCreateInfo()};
    instanceInfo.pNext = &messengerInfo;
    instanceInfo.enabledLayerCount = layerInfo.validationLayers.size();
    instanceInfo.ppEnabledLayerNames = layerInfo.validationLayers.data();
#else
    instanceInfo.pNext = nullptr;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
#endif

    InstanceRef instance;
    instance.create(instanceInfo);
    return instance;
}