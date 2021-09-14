//
// Created by jack on 6/21/21.
//

#include "device.h"
#include "window.h"
#include "swapchain.h"

#include <algorithm>
#include <set>
#include <piperine/device.h>


using namespace pip;

bool QueueProperties::isViable() const {
    return graphicsFamily.has_value() && presentFamily.has_value();
}

Device pip::createDevice(const InstanceRef &instance, const SurfaceRef &surface) {
    Device device;

    uint32_t deviceCount;
    std::vector<VkPhysicalDevice> devices;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
    devices.resize(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

    assert(!devices.empty());

    std::vector<std::pair<VkPhysicalDevice, uint32_t>> deviceRankings(devices.size());
    std::transform(devices.cbegin(), devices.cend(), std::back_inserter(deviceRankings), [&surface](VkPhysicalDevice device){
        return std::pair<VkPhysicalDevice, uint32_t>(device, detail::rankDevice(device, *surface));
    });
    std::sort(deviceRankings.begin(), deviceRankings.end(), [](auto& lhs, auto& rhs){
        return lhs.second < rhs.second;
    });
    assert(deviceRankings.back().second > 0);
    device.physical = deviceRankings.back().first;

    auto queueProperties = detail::getQueueProperties(device.physical, *surface);
    float queuePriority = 1.f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfo.queueFamilyIndex = queueProperties.graphicsFamily.value();
    queueCreateInfos.push_back(queueCreateInfo);
    if (queueProperties.graphicsFamily.value() != queueProperties.presentFamily.value()) {
        queueCreateInfo.queueFamilyIndex = queueProperties.presentFamily.value();
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    #ifdef PIPERINE_ENABLE_VALIDATION
    auto layers = getValidationLayers();
    deviceCreateInfo.enabledLayerCount = layers.size();
    deviceCreateInfo.ppEnabledLayerNames = layers.data();
    auto extensions = getDeviceExtensions();
    deviceCreateInfo.enabledExtensionCount = extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();
    #else
    deviceCreateInfo.enabledLayerCount = 0;
    #endif

    device.logical.create(device.physical, deviceCreateInfo);

    vkGetDeviceQueue(*device.logical, queueProperties.graphicsFamily.value(), 0, &device.graphicsQueue);
    vkGetDeviceQueue(*device.logical, queueProperties.presentFamily.value(), 0, &device.presentQueue);
    device.graphicsQueueIndex = queueProperties.graphicsFamily.value();
    device.presentQueueIndex = queueProperties.presentFamily.value();

    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_0;
    allocatorInfo.physicalDevice = device.physical;
    allocatorInfo.device = *device.logical;
    allocatorInfo.instance = *instance;
    device.allocator.create(allocatorInfo);

    VkCommandPoolCreateInfo poolCreateInfo{};
    poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreateInfo.queueFamilyIndex = device.graphicsQueueIndex;
    poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    device.pool.create(device.logical, poolCreateInfo);

    return device;
}

uint32_t pip::detail::rankDevice(VkPhysicalDevice physical, VkSurfaceKHR surface) {
    uint32_t ranking = 1;

    auto queueProperties = getQueueProperties(physical, surface);
    if (!queueProperties.isViable()) return 0;
    if (queueProperties.presentFamily == queueProperties.graphicsFamily) ranking += combinedQueueRank;

    uint32_t extensionCount;
    std::vector<VkExtensionProperties> extensionProperties;
    vkEnumerateDeviceExtensionProperties(physical, nullptr, &extensionCount, nullptr);
    extensionProperties.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physical, nullptr, &extensionCount, extensionProperties.data());

    auto extensionsVector = getDeviceExtensions();
    std::set<std::string> extensions{extensionsVector.begin(), extensionsVector.end()};
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

QueueProperties pip::detail::getQueueProperties(VkPhysicalDevice physical, VkSurfaceKHR surface) {
    QueueProperties queueProperties;
    uint32_t queueFamilyCount = 0;
    std::vector<VkQueueFamilyProperties> queuePropertiesVector;
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueFamilyCount, nullptr);
    queuePropertiesVector.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical, &queueFamilyCount, queuePropertiesVector.data());

    for (size_t i = 0; i < queuePropertiesVector.size(); i++) {
        auto& prop = queuePropertiesVector.at(i);
        if (prop.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queueProperties.graphicsFamily = i;
        }

        VkBool32 present = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &present);
        if (present) queueProperties.presentFamily = i;

        if (queueProperties.isViable()) break;
    }

    return queueProperties;
}
