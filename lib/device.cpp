//
// Created by jack on 8/22/21.
//

#include <piperine/device.h>

#include <set>

using namespace pip;
using namespace pip::detail;

VkPhysicalDevice detail::getPhysical(const InstanceRef& instance, const SurfaceRef& surface, const ExtensionLayerInfo& layerInfo) {
    uint32_t deviceCount;
    std::vector<VkPhysicalDevice> devices;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
    devices.resize(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

    assert(!devices.empty());

    std::vector<std::pair<VkPhysicalDevice, uint32_t>> deviceRankings(devices.size());
    std::transform(devices.cbegin(), devices.cend(), std::back_inserter(deviceRankings), [&surface, &layerInfo](VkPhysicalDevice device){
        return std::pair<VkPhysicalDevice, uint32_t>(device, rankDevice(device, *surface, layerInfo));
    });
    std::sort(deviceRankings.begin(), deviceRankings.end(), [](auto& lhs, auto& rhs){
        return lhs.second < rhs.second;
    });
    assert(deviceRankings.back().second > 0);
    return deviceRankings.back().first;
}

uint32_t detail::rankDevice(VkPhysicalDevice physical, VkSurfaceKHR surface, const ExtensionLayerInfo& layerInfo) {
    uint32_t ranking = 1;

    auto queueFamilies = getQueueFamilies(physical, surface);
    if (!isQueueViable(queueFamilies)) return 0;
    if (queueFamilies.present == queueFamilies.graphics) ranking += combinedQueueRank;

    uint32_t extensionCount;
    std::vector<VkExtensionProperties> extensionProperties;
    vkEnumerateDeviceExtensionProperties(physical, nullptr, &extensionCount, nullptr);
    extensionProperties.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physical, nullptr, &extensionCount, extensionProperties.data());

    std::set<std::string> extensions{layerInfo.deviceExtensions.begin(), layerInfo.deviceExtensions.end()};
    for (const auto& prop : extensionProperties) {
        extensions.erase(prop.extensionName);
    }

    if (!extensions.empty()) return 0;

    auto swapChainInfo = getSwapchainSupportInfo(physical, surface);
    if (swapChainInfo.formats.empty() || swapChainInfo.presentModes.empty()) return 0;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physical, &properties);
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) ranking += dedicatedGraphicsRank;

    return ranking;
}

bool detail::isQueueViable(const QueueFamilies &families) {
    return families.graphics.has_value() && families.present.has_value();
}

QueueFamilies detail::getQueueFamilies(VkPhysicalDevice physical, VkSurfaceKHR surface) {
    QueueFamilies queueFamilies;
    uint32_t queueFamilyCount = 0;
    std::vector<VkQueueFamilyProperties> queuePropertiesVector;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueFamilyCount, nullptr);
    queuePropertiesVector.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueFamilyCount, queuePropertiesVector.data());

    for (size_t i = 0; i < queuePropertiesVector.size(); i++) {
        auto& prop = queuePropertiesVector.at(i);
        if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueFamilies.graphics = i;
        }

        VkBool32 present = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &present);
        if (present) queueFamilies.present = i;

        if (isQueueViable(queueFamilies)) break;
    }

    return queueFamilies;
}

SwapchainSupportInfo detail::getSwapchainSupportInfo(VkPhysicalDevice physical, VkSurfaceKHR surface) {
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

Queues detail::getQueues(VkPhysicalDevice physical, const DeviceRef& device, const SurfaceRef &surface) {
    Queues queues;
    auto queueFamilies = getQueueFamilies(physical, *surface);
    vkGetDeviceQueue(*device, queueFamilies.graphics.value(), 0, &queues.graphics);
    vkGetDeviceQueue(*device, queueFamilies.present.value(), 0, &queues.present);
    queues.graphicsIndex = queueFamilies.graphics.value();
    queues.presentIndex = queueFamilies.present.value();
    return queues;
}

DeviceRef detail::createDevice(VkPhysicalDevice physical, const SurfaceRef& surface, const ExtensionLayerInfo& layerInfo) {
    auto queueFamilies = getQueueFamilies(physical, *surface);
    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfo.queueFamilyIndex = queueFamilies.graphics.value();
    queueCreateInfos.push_back(queueCreateInfo);
    if (queueFamilies.graphics.value() != queueFamilies.present.value()) {
        queueCreateInfo.queueFamilyIndex = queueFamilies.present.value();
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = layerInfo.deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = layerInfo.deviceExtensions.data();
#ifdef PIPERINE_ENABLE_VALIDATION
    deviceCreateInfo.enabledLayerCount = layerInfo.validationLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = layerInfo.validationLayers.data();
#else
    deviceCreateInfo.enabledLayerCount = 0;
#endif

    DeviceRef device;
    device.create(physical, deviceCreateInfo);
    return device;
}
