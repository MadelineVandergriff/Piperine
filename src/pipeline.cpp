//
// Created by jack on 6/22/21.
//

#include "pipeline.h"
#include "commands.h"
#include "buffers.h"

using namespace pip;

Pipeline pip::createPipeline(Device& device, Swapchain &swapchain, const PipelineGraph& graph) {
    Pipeline pipeline;
    pipeline.graph = graph;

    for (const auto& shaderDescriptor : graph.shaderDescriptors) {
        pipeline.shaders.emplace(shaderDescriptor.first, detail::createShader(device.logical, shaderDescriptor.second));
    }

    //TODO grab proper shaders
    auto vertexKey = graph.pipelineDescriptors.at(0).vertexShaderKey;
    auto fragmentKey = graph.pipelineDescriptors.at(0).fragmentShaderKey;

    VkPipelineShaderStageCreateInfo shaderStages[] =
            {detail::getShaderStageCreateInfo(pipeline.shaders.at(vertexKey)),
            detail::getShaderStageCreateInfo(pipeline.shaders.at(fragmentKey))};

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.vertexBindingDescriptionCount = pipeline.shaders.at(vertexKey).bindings.size();
    vertexInputCreateInfo.pVertexBindingDescriptions = pipeline.shaders.at(vertexKey).bindings.data();
    vertexInputCreateInfo.vertexAttributeDescriptionCount = pipeline.shaders.at(vertexKey).attributes.size();
    vertexInputCreateInfo.pVertexAttributeDescriptions = pipeline.shaders.at(vertexKey).attributes.data();

    VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo{};
    assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain.extent.width;
    viewport.height = swapchain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = swapchain.extent;
    scissor.offset = {0, 0};

    VkPipelineViewportStateCreateInfo viewportCreateInfo{};
    viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportCreateInfo.viewportCount = 1;
    viewportCreateInfo.pViewports = &viewport;
    viewportCreateInfo.scissorCount = 1;
    viewportCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
    rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationCreateInfo.lineWidth = 1.0f;
    rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationCreateInfo.depthBiasClamp = 0.0f;
    rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleCreateInfo.minSampleShading = 1.0f;
    multisampleCreateInfo.pSampleMask = nullptr;
    multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendState{};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendState.blendEnable = VK_FALSE;
    colorBlendState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo{};
    colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlendingCreateInfo.attachmentCount = 1;
    colorBlendingCreateInfo.pAttachments = &colorBlendState;
    colorBlendingCreateInfo.blendConstants[0] = 0.0f; // Optional
    colorBlendingCreateInfo.blendConstants[1] = 0.0f; // Optional
    colorBlendingCreateInfo.blendConstants[2] = 0.0f; // Optional
    colorBlendingCreateInfo.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkPushConstantRange pushConstants{};
    if (size_t size = graph.shaderDescriptors.at(vertexKey).pushConstantSize.value_or(0)) {
        pushConstants.size = size;
        pushConstants.offset = 0;
        pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        layoutCreateInfo.pushConstantRangeCount = 1;
        layoutCreateInfo.pPushConstantRanges = &pushConstants;
    } else {
        layoutCreateInfo.pushConstantRangeCount = 0;
        layoutCreateInfo.pPushConstantRanges = nullptr;
    }

    if (size_t size = graph.shaderDescriptors.at(fragmentKey).uniformSize.value_or(0)) {
        VkDescriptorSetLayoutBinding uniformBinding{};
        uniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        uniformBinding.binding = 0;
        uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBinding.pImmutableSamplers = nullptr;
        uniformBinding.descriptorCount = 1;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &uniformBinding;

        pipeline.descriptorSetLayout.create(device.logical, descriptorSetLayoutCreateInfo);

        auto descriptorSetLayout = *pipeline.descriptorSetLayout;
        layoutCreateInfo.setLayoutCount = 1;
        layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipeline.layout.create(device.logical, layoutCreateInfo);

        BufferInfo bufferInfo{
            .size = size,
            .bufferFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .memoryFlags = VMA_MEMORY_USAGE_CPU_TO_GPU
        };

        for (const auto& _ : swapchain.views) {
            pipeline.uniformBuffers.push_back(createBuffer(device.allocator, bufferInfo));
        }

        VkDescriptorPoolSize poolSize{};
        poolSize.descriptorCount = pipeline.uniformBuffers.size();
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;
        descriptorPoolCreateInfo.maxSets = pipeline.uniformBuffers.size();
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pipeline.descriptorPool.create(device.logical, descriptorPoolCreateInfo);

        std::vector<VkDescriptorSetLayout> setLayouts(pipeline.uniformBuffers.size(), *pipeline.descriptorSetLayout);
        VkDescriptorSetAllocateInfo setInfo{};
        setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        setInfo.descriptorPool = *pipeline.descriptorPool;
        setInfo.descriptorSetCount = setLayouts.size();
        setInfo.pSetLayouts = setLayouts.data();
        pipeline.descriptorSets.allocate(device.logical, pipeline.descriptorPool, setInfo);

        for (size_t i = 0; i < pipeline.uniformBuffers.size(); i++) {
            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = pipeline.uniformBuffers.at(i)->buffer;
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = size;

            VkWriteDescriptorSet writeDescriptorSet{};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstSet = pipeline.descriptorSets->at(i);
            writeDescriptorSet.dstBinding = 0;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
            writeDescriptorSet.pImageInfo = nullptr;
            writeDescriptorSet.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(*device.logical, 1, &writeDescriptorSet, 0, nullptr);
        }
    } else {
        layoutCreateInfo.setLayoutCount = 0;
        layoutCreateInfo.pSetLayouts = nullptr;
        pipeline.layout.create(device.logical, layoutCreateInfo);
    }

    pipeline.renderPass = detail::createRenderPass(device.logical, swapchain, graph.pipelineDescriptors.at(0).renderPass);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &assemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
    pipelineCreateInfo.pDepthStencilState = nullptr;
    pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
    pipelineCreateInfo.pDynamicState = nullptr;
    pipelineCreateInfo.layout = *pipeline.layout;
    pipelineCreateInfo.renderPass = *pipeline.renderPass; // TODO more render passes, don't forget subpass
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    pipeline.pipeline.create(device.logical, pipelineCreateInfo);

    pipeline.framebuffers.resize(swapchain.views.size());
    for (size_t i = 0; i < pipeline.framebuffers.size(); i++) {
        VkImageView attachments[] = {
                *swapchain.views.at(i)
        };

        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = *pipeline.renderPass; // TODO see above
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = attachments;
        framebufferCreateInfo.width = swapchain.extent.width;
        framebufferCreateInfo.height = swapchain.extent.height;
        framebufferCreateInfo.layers = 1;

        pipeline.framebuffers.at(i).create(device.logical, framebufferCreateInfo);
    }

    pipeline.commandBuffers = detail::allocateCommandBuffers(device.logical, device.pool, pipeline.framebuffers.size());
    pipeline.extent = swapchain.extent;



    return pipeline;
}

Shader detail::createShader(const DeviceRef& device, const ShaderDescriptor &descriptor) {
    Shader shader;
    auto bytecode = readFile(descriptor.path);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode.data());
    createInfo.codeSize = bytecode.size();
    shader.module.create(device, createInfo);
    shader.type = descriptor.type;

    for (const auto& binding : descriptor.bindings) {
        shader.bindings.push_back(binding.binding);
        std::copy(std::begin(binding.attributes), std::end(binding.attributes), std::back_inserter(shader.attributes));
    }

    return shader;
}

RenderPassRef detail::createRenderPass(const DeviceRef &device, const Swapchain& swapchain, const RenderPassDescriptor &descriptor) {
    RenderPassRef renderPass;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchain.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorRef.attachment = 0;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = 1;
    renderPassCreateInfo.pAttachments = &colorAttachment;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    renderPass.create(device, renderPassCreateInfo);
    return renderPass;
}

VkPipelineShaderStageCreateInfo detail::getShaderStageCreateInfo(const Shader &shader) {
    VkPipelineShaderStageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.module = *shader.module;
    createInfo.pName = "main";
    switch (shader.type) {
        case ShaderType::VERTEX:
            createInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderType::FRAGMENT:
            createInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderType::COMPUTE:
            createInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
    }

    return createInfo;
}

CommandBufferVector detail::allocateCommandBuffers(const DeviceRef& device, const CommandPoolRef& pool, uint32_t numBuffers) {
    CommandBufferVector buffers;

    VkCommandBufferAllocateInfo bufferAllocateInfo{};
    bufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    bufferAllocateInfo.commandPool = *pool;
    bufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    bufferAllocateInfo.commandBufferCount = numBuffers;

    buffers.allocate(device, pool, bufferAllocateInfo);
    //assert(vkAllocateCommandBuffers(*device, &bufferAllocateInfo, buffers.data()) == VK_SUCCESS);
    return buffers;
}
