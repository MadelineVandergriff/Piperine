//
// Created by jack on 6/16/21.
//

#include "piperine/vulkan_types.h"

using namespace pip::detail;

InstanceRef_ Instance::create(VkInstanceCreateInfo info) {
    VkInstance instance;
    vkCreateInstance(&info, nullptr, &instance);
    return {static_cast<void*>(instance), [](void* instance){
        log("Destroying Instance", LogType::DESTRUCTION);
        vkDestroyInstance(static_cast<VkInstance>(instance), nullptr);
    }};
}

void InstanceRef::create(VkInstanceCreateInfo info) {
    VkInstance instance;
    vkCreateInstance(&info, nullptr, &instance);
    underlying = {instance, [](VkInstance instance){
        log("Destroying Instance", LogType::DESTRUCTION);
        vkDestroyInstance(instance, nullptr);
    }};
}

void WindowRef::create(WindowCreateInfo info) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, (info.flags & WindowFlags::RESIZABLE) ? GLFW_TRUE : GLFW_FALSE);
    underlying = {glfwCreateWindow(info.width, info.height, info.title, nullptr, nullptr), [](GLFWwindow* window){
        log("Destroying Window", LogType::DESTRUCTION);
        glfwDestroyWindow(window);
    }};
}

void SurfaceRef::create(InstanceRef instance, const WindowRef& window) {
    VkSurfaceKHR surface;
    assert(glfwCreateWindowSurface(*instance, *window, nullptr, &surface) == VK_SUCCESS);
    underlying = {surface, [instance = std::move(instance)](VkSurfaceKHR surface){
        log("Destroying Surface", LogType::DESTRUCTION);
        vkDestroySurfaceKHR(*instance, surface, nullptr);
    }};
}

void DeviceRef::create(VkPhysicalDevice phys, VkDeviceCreateInfo info) {
    VkDevice device;
    assert(vkCreateDevice(phys, &info, nullptr, &device) == VK_SUCCESS);
    underlying = {device, [](VkDevice device){
        log("Destroying Device", LogType::DESTRUCTION);
        vkDestroyDevice(device, nullptr);
    }};
}

void SwapchainRef::create(DeviceRef device, VkSwapchainCreateInfoKHR info) {
    VkSwapchainKHR swapchain;
    assert(vkCreateSwapchainKHR(*device, &info, nullptr, &swapchain) == VK_SUCCESS);
    underlying = {swapchain, [device = std::move(device)](VkSwapchainKHR swapchain){
        log("Destroying Swapchain", LogType::DESTRUCTION);
        vkDestroySwapchainKHR(*device, swapchain, nullptr);
    }};
}

void ImageViewRef::create(DeviceRef device, VkImageViewCreateInfo info) {
    VkImageView imageView;
    assert(vkCreateImageView(*device, &info, nullptr, &imageView) == VK_SUCCESS);
    underlying = {imageView, [device = std::move(device)](VkImageView imageView){
        log("Destroying ImageView", LogType::DESTRUCTION);
        vkDestroyImageView(*device, imageView, nullptr);
    }};
}

void ShaderModuleRef::create(DeviceRef device, VkShaderModuleCreateInfo info) {
    VkShaderModule shaderModule;
    assert(vkCreateShaderModule(*device, &info, nullptr, &shaderModule) == VK_SUCCESS);
    underlying = {shaderModule, [device = std::move(device)](VkShaderModule shaderModule){
        log("Destroying Shader Module", LogType::DESTRUCTION);
        vkDestroyShaderModule(*device, shaderModule, nullptr);
    }};
}

void PipelineLayoutRef::create(DeviceRef device, VkPipelineLayoutCreateInfo info) {
    VkPipelineLayout layout;
    assert(vkCreatePipelineLayout(*device, &info, nullptr, &layout) == VK_SUCCESS);
    underlying = {layout, [device = std::move(device)](VkPipelineLayout layout){
        log("Destroying Pipeline Layout", LogType::DESTRUCTION);
        vkDestroyPipelineLayout(*device, layout, nullptr);
    }};
}

void PipelineRef::create(DeviceRef device, VkGraphicsPipelineCreateInfo info) {
    VkPipeline pipeline;
    assert(vkCreateGraphicsPipelines(*device, nullptr, 1, &info, nullptr, &pipeline) == VK_SUCCESS);
    underlying = {pipeline, [device = std::move(device)](VkPipeline pipeline){
        log("Destroying Pipeline", LogType::DESTRUCTION);
        vkDestroyPipeline(*device, pipeline, nullptr);
    }};
}

void RenderPassRef::create(DeviceRef device, VkRenderPassCreateInfo info) {
    VkRenderPass renderPass;
    assert(vkCreateRenderPass(*device, &info, nullptr, &renderPass) == VK_SUCCESS);
    underlying = {renderPass, [device = std::move(device)](VkRenderPass renderPass){
        log("Destroying Render Pass", LogType::DESTRUCTION);
        vkDestroyRenderPass(*device, renderPass, nullptr);
    }};
}

void FramebufferRef::create(DeviceRef device, VkFramebufferCreateInfo info) {
    VkFramebuffer framebuffer;
    assert(vkCreateFramebuffer(*device, &info, nullptr, &framebuffer) == VK_SUCCESS);
    underlying = {framebuffer, [device = std::move(device)](VkFramebuffer framebuffer){
        log("Destroying Framebuffer", LogType::DESTRUCTION);
        vkDestroyFramebuffer(*device, framebuffer, nullptr);
    }};
}

void CommandPoolRef::create(DeviceRef device, VkCommandPoolCreateInfo info) {
    VkCommandPool commandPool;
    assert(vkCreateCommandPool(*device, &info, nullptr, &commandPool) == VK_SUCCESS);
    underlying = {commandPool, [device = std::move(device)](VkCommandPool commandPool){
        log("Destroying Command Pool", LogType::DESTRUCTION);
        vkDestroyCommandPool(*device, commandPool, nullptr);
    }};
}

void DebugUtilsMessengerRef::create(InstanceRef instance, VkDebugUtilsMessengerCreateInfoEXT info) {
    VkDebugUtilsMessengerEXT debugUtilsMessenger;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            *instance,
            "vkCreateDebugUtilsMessengerEXT");
    assert(func != nullptr && "vkCreateDebugUtilsMessengerEXT not found");
    assert(func(*instance, &info, nullptr, &debugUtilsMessenger) == VK_SUCCESS);
    underlying = {debugUtilsMessenger, [instance = std::move(instance)](VkDebugUtilsMessengerEXT debugUtilsMessenger){
        log("Destroying Debug Utils Messenger", LogType::DESTRUCTION);
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                *instance,
                "vkDestroyDebugUtilsMessengerEXT");
        assert(func != nullptr && "vkDestroyDebugUtilsMessengerEXT not found");
        func(*instance, debugUtilsMessenger, nullptr);
    }};
}

void SemaphoreRef::create(DeviceRef device, VkSemaphoreCreateInfo info) {
    VkSemaphore semaphore;
    assert(vkCreateSemaphore(*device, &info, nullptr, &semaphore) == VK_SUCCESS);
    underlying = {semaphore, [device = std::move(device)](VkSemaphore semaphore){
        log("Destroying Semaphore", LogType::DESTRUCTION);
        vkDestroySemaphore(*device, semaphore, nullptr);
    }};
}

void FenceRef::create(DeviceRef device, VkFenceCreateInfo info) {
    VkFence fence;
    assert(vkCreateFence(*device, &info, nullptr, &fence) == VK_SUCCESS);
    underlying = {fence, [device = std::move(device)](VkFence fence){
        log("Destroying Fence", LogType::DESTRUCTION);
        vkDestroyFence(*device, fence, nullptr);
    }};
}

void BufferRef::create(DeviceRef device, VkBufferCreateInfo info) {
    VkBuffer buffer;
    assert(vkCreateBuffer(*device, &info, nullptr, &buffer) == VK_SUCCESS);
    underlying = {buffer, [device = std::move(device)](VkBuffer buffer){
        log("Destroying Buffer", LogType::DESTRUCTION);
        vkDestroyBuffer(*device, buffer, nullptr);
    }};
}

void DeviceMemoryRef::allocate(DeviceRef device, VkMemoryAllocateInfo info) {
    VkDeviceMemory memory;
    assert(vkAllocateMemory(*device, &info, nullptr, &memory) == VK_SUCCESS);
    underlying = {memory, [device = std::move(device)](VkDeviceMemory memory){
        log("Destroying Device Memory", LogType::DESTRUCTION);
        vkFreeMemory(*device, memory, nullptr);
    }};
}

void CommandBufferVector::allocate(DeviceRef device, CommandPoolRef pool, VkCommandBufferAllocateInfo info) {
    assert(info.commandPool == *pool);
    auto buffers = new std::vector<VkCommandBuffer>(info.commandBufferCount);
    assert(vkAllocateCommandBuffers(*device, &info, buffers->data()) == VK_SUCCESS);
    underlying = {buffers, [device = std::move(device), pool = std::move(pool)](std::vector<VkCommandBuffer>* buffers){
        log("Destroying Command Buffers", LogType::DESTRUCTION);
        vkFreeCommandBuffers(*device, *pool, buffers->size(), buffers->data());
        delete buffers;
    }};
}

void CommandBufferRef::allocate(DeviceRef device, CommandPoolRef pool, VkCommandBufferAllocateInfo info) {
    assert(info.commandBufferCount == 1);
    assert(info.commandPool == *pool);
    VkCommandBuffer buffer;
    assert(vkAllocateCommandBuffers(*device, &info, &buffer) == VK_SUCCESS);
    underlying = {buffer, [device = std::move(device), pool = std::move(pool)](VkCommandBuffer buffer){
        log("Destroying Command Buffer", LogType::DESTRUCTION);
        vkFreeCommandBuffers(*device, *pool, 1, &buffer);
    }};
}

void AllocatorRef::create(VmaAllocatorCreateInfo info) {
    VmaAllocator allocator;
    assert(vmaCreateAllocator(&info, &allocator) == VK_SUCCESS);
    underlying = {allocator, [](VmaAllocator allocator){
        log("Destroying Allocator", LogType::DESTRUCTION);
        vmaDestroyAllocator(allocator);
    }};
}

void VmaBufferRef::create(AllocatorRef allocator, VkBufferCreateInfo bufferInfo, VmaAllocationCreateInfo allocInfo) {
    auto buffer = new VmaBuffer();
    buffer->size = bufferInfo.size;
    assert(vmaCreateBuffer(*allocator, &bufferInfo, &allocInfo, &buffer->buffer, &buffer->allocation, nullptr) == VK_SUCCESS);
    underlying = {buffer, [allocator = std::move(allocator)](VmaBuffer* buffer){
        log("Destroying VMA Buffer", LogType::DESTRUCTION);
        vmaDestroyBuffer(*allocator, buffer->buffer, buffer->allocation);
    }};
}

void DescriptorSetLayoutRef::create(DeviceRef device, VkDescriptorSetLayoutCreateInfo info) {
    VkDescriptorSetLayout layout;
    assert(vkCreateDescriptorSetLayout(*device, &info, nullptr, &layout) == VK_SUCCESS);
    underlying = {layout, [device = std::move(device)](VkDescriptorSetLayout layout){
        log("Destroying Descriptor Set Layout", LogType::DESTRUCTION);
        vkDestroyDescriptorSetLayout(*device, layout, nullptr);
    }};
}

void DescriptorPoolRef::create(DeviceRef device, VkDescriptorPoolCreateInfo info) {
    VkDescriptorPool pool;
    assert(vkCreateDescriptorPool(*device, &info, nullptr, &pool) == VK_SUCCESS);
    underlying = {pool, [device = std::move(device)](VkDescriptorPool pool){
        log("Destroying Descriptor Pool", LogType::DESTRUCTION);
        vkDestroyDescriptorPool(*device, pool, nullptr);
    }};
}

void DescriptorSetRef::allocate(DeviceRef device, DescriptorPoolRef pool, VkDescriptorSetAllocateInfo info) {
    assert(info.descriptorSetCount == 1);
    assert(info.descriptorPool == *pool);
    VkDescriptorSet set;
    assert(vkAllocateDescriptorSets(*device, &info, &set) == VK_SUCCESS);
    underlying = {set, [device = std::move(device), pool = std::move(pool)](VkDescriptorSet set){
        log("Destroying Descriptor Set", LogType::DESTRUCTION);
        vkFreeDescriptorSets(*device, *pool, 1, &set);
    }};
}

void DescriptorSetVector::allocate(DeviceRef device, DescriptorPoolRef pool, VkDescriptorSetAllocateInfo info) {
    assert(info.descriptorPool == *pool);
    auto sets = new std::vector<VkDescriptorSet>(info.descriptorSetCount);
    assert(vkAllocateDescriptorSets(*device, &info, sets->data()) == VK_SUCCESS);
    underlying = {sets, [device = std::move(device), pool = std::move(pool)](std::vector<VkDescriptorSet>* sets){
        log("Destroying Descriptor Sets", LogType::DESTRUCTION);
        vkFreeDescriptorSets(*device, *pool, sets->size(), sets->data());
        delete sets;
    }};
}

void VmaImageRef::create(AllocatorRef allocator, VkImageCreateInfo imageInfo, VmaAllocationCreateInfo allocInfo) {
    auto image = new VmaImage();
    assert(vmaCreateImage(*allocator, &imageInfo, &allocInfo, &image->image, &image->allocation, nullptr) == VK_SUCCESS);
    underlying = {image, [allocator = std::move(allocator)](VmaImage* image){
        log("Destroying VMA Buffer", LogType::DESTRUCTION);
        vmaDestroyImage(*allocator, image->image, image->allocation);
    }};
}

GLFWSingleton::GLFWSingleton() {
    glfwInit();
}

GLFWSingleton::~GLFWSingleton() {
    glfwTerminate();
}


