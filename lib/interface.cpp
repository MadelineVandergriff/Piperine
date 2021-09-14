//
// Created by jack on 7/30/21.
//

#include <piperine/vulkan_utility.h>
#include <piperine/interface.h>
#include <piperine/instance.h>
#include <piperine/messenger.h>
#include <piperine/device.h>
#include <piperine/allocators.h>
#include <piperine/swapchain.h>

#include <memory>

using namespace pip;
using namespace pip::detail;

void detail::genericInternalsDeleter(GenericInternals* ptr) {
    assert(ptr);
    delete ptr;
}

ExtensionLayerInfo detail::getExtensionLayerInfo(const ExtensionLayerRetrievalInfo& info) {
    GLFWSingleton::getSingleton();
    uint32_t glfwExtensionCount;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    ExtensionLayerInfo layerInfo{
        .instanceExtensions = {glfwExtensions, glfwExtensions + glfwExtensionCount},
        .deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
        .validationLayers = {"VK_LAYER_KHRONOS_validation"}
    };
#ifdef PIPERINE_ENABLE_VALIDATION
    layerInfo.instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return layerInfo;
}

GenericInternalsPtr detail::genericInitialization(const GenericInternalsCreateInfo& info) {
    ExtensionLayerInfo layerInfo = getExtensionLayerInfo(ExtensionLayerRetrievalInfo{});
    GenericInternals internals;
    internals.instance = createInstance(info.windowInfo, layerInfo);
    internals.messenger = createMessenger(internals.instance);
    internals.window.create(info.windowInfo);
    internals.surface.create(internals.instance, internals.window);
    internals.physical = getPhysical(internals.instance, internals.surface, layerInfo);
    internals.device = createDevice(internals.physical, internals.surface, layerInfo);
    internals.queues = getQueues(internals.physical, internals.device, internals.surface);
    internals.allocator = createAllocator(internals.instance, internals.physical, internals.device);
    internals.cmdPool = createCommandPool(internals.device, internals.queues);
    auto [swap, data] = createSwapchain(internals.window, internals.surface, internals.physical, internals.device, internals.queues);
    internals.swapchain = std::move(swap);
    internals.swapchainData = std::move(data);

    return GenericInternalsPtr{new GenericInternals{std::move(internals)}, genericInternalsDeleter};
}

