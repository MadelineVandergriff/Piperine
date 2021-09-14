//
// Created by jack on 6/22/21.
//

#include "application.h"

#include "window.h"
#include "device.h"
#include "swapchain.h"
#include "pipeline.h"
#include "commands.h"
#include "buffers.h"
#include "shader_types.h"

using namespace pip;

Application::Application(WindowInfo windowInfo, PipelineGraph _graph, std::function<void(Application&)> _frameCallback) {
    graph = std::move(_graph);
    frameCallback = std::move(_frameCallback);

    auto [_instance, _messenger] = InitializeVulkan(windowInfo.title);
    instance = _instance;
    messenger = _messenger;

    window = createWindow(instance, windowInfo);
    device = createDevice(instance, window.surface);
    swapchain = createSwapchain(device, window);
    pipeline = createPipeline(device, swapchain, graph);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    availableImageSemaphores.resize(detail::framesInFlight);
    finishedRenderSemaphores.resize(detail::framesInFlight);
    imageInFlightFences.resize(detail::framesInFlight);
    imagesInFlight.resize(swapchain.images.size(), VK_NULL_HANDLE);

    for (auto& semaphore : availableImageSemaphores) {
        semaphore.create(device.logical, semaphoreCreateInfo);
    }

    for (auto& semaphore : finishedRenderSemaphores) {
        semaphore.create(device.logical, semaphoreCreateInfo);
    }

    for (auto& fence : imageInFlightFences) {
        fence.create(device.logical, fenceCreateInfo);
    }
}

void pip::drawFrame(Application &app) {
    auto pImageFence = *app.imageInFlightFences.at(app.frameModulus);
    auto pRenderSemaphore = *app.finishedRenderSemaphores.at(app.frameModulus);
    auto pImageSemaphore = *app.availableImageSemaphores.at(app.frameModulus);

    vkWaitForFences(*app.device.logical, 1, &pImageFence, VK_TRUE, UINT64_MAX);

    uint32_t imgIndex;
    auto result = vkAcquireNextImageKHR(*app.device.logical, *app.swapchain.swapchain, UINT64_MAX, pImageSemaphore, VK_NULL_HANDLE, &imgIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        recreateSwapchain(app);
        app.actualFrame++;
        return;
    }

    if (app.imagesInFlight.at(imgIndex) != VK_NULL_HANDLE) {
        vkWaitForFences(*app.device.logical, 1, &app.imagesInFlight.at(imgIndex), VK_TRUE, UINT64_MAX);
    }
    app.imagesInFlight.at(imgIndex) = pImageFence;
    vkResetFences(*app.device.logical, 1, &pImageFence);

    auto currentBuffer = startCommandBuffer(app.pipeline, imgIndex);
    if (!app.pipeline.uniformBuffers.empty()) {
        vkCmdBindDescriptorSets(
            currentBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            *app.pipeline.layout,
            0,
            1,
            &app.pipeline.descriptorSets->at(imgIndex),
            0,
            nullptr
        );
    }
    for (const auto& descriptor : app.draws) {
        drawCommandBuffer(currentBuffer, app.pipeline, descriptor.second);
    }
    endCommandBuffer(currentBuffer);

    if (!app.uniformDescriptors.empty() && !app.pipeline.uniformBuffers.empty()) {
        if (imgIndex == app.uniformUpdateIndex) {
            app.uniformDescriptors.pop_front();
            app.uniformUpdateIndex = UINT32_MAX;
        } else {
            populateBuffer(app.device.allocator, app.pipeline.uniformBuffers.at(imgIndex), app.uniformDescriptors.front());
            app.uniformUpdateIndex = imgIndex;
        }
    }

    VkSubmitInfo submitInfo{};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &pImageSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &pRenderSemaphore;

    assert(vkQueueSubmit(app.device.graphicsQueue, 1, &submitInfo, pImageFence) == VK_SUCCESS);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &pRenderSemaphore;
    presentInfo.swapchainCount = 1;
    auto pSwapchain = *app.swapchain.swapchain;
    presentInfo.pSwapchains = &pSwapchain;
    presentInfo.pImageIndices = &imgIndex;
    presentInfo.pResults = nullptr;

    vkQueuePresentKHR(app.device.presentQueue, &presentInfo);
    app.frameModulus++;
    app.actualFrame++;
    app.frameModulus %= detail::framesInFlight;

    app.frameCallback(app);
}

void pip::recreateSwapchain(Application &app) {
    vkDeviceWaitIdle(*app.device.logical);

    int width = 0, height = 0;
    glfwGetFramebufferSize(*app.window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(*app.window.window, &width, &height);
        glfwWaitEvents();
    }

    app.pipeline = Pipeline{};
    app.swapchain = Swapchain{};
    app.swapchain = createSwapchain(app.device, app.window);
    app.pipeline = createPipeline(app.device, app.swapchain, app.graph);
}
