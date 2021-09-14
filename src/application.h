//
// Created by jack on 6/22/21.
//

#ifndef PIPERINE_APPLICATION_H
#define PIPERINE_APPLICATION_H

#include "vulkan_containers.h"
#include "shader_types.h"
#include "mesh.h"

#include <deque>

namespace pip {
    struct Application {
        Application(WindowInfo windowInfo, PipelineGraph graph,
                    std::function<void(Application&)> frameCallback = [](auto&){});

        InstanceRef instance;
        DebugUtilsMessengerRef messenger;
        Window window;
        Device device;
        Swapchain swapchain;
        Pipeline pipeline;

        std::vector<SemaphoreRef> availableImageSemaphores;
        std::vector<SemaphoreRef> finishedRenderSemaphores;
        std::vector<FenceRef> imageInFlightFences;
        std::vector<VkFence> imagesInFlight;

        std::deque<GenericData> uniformDescriptors;
        uint32_t uniformUpdateIndex{UINT32_MAX};

        size_t frameModulus{0};
        size_t actualFrame{0};

        PipelineGraph graph;
        std::function<void(Application&)> frameCallback;

        std::map<size_t, DrawDescriptor> draws;
        size_t drawCount{0};
    };

    void drawFrame(Application& app);
    void recreateSwapchain(Application& app);

    template<typename T>
    void registerModel(Application& app, Model<T>& model) {
        app.draws.emplace(app.drawCount, getDrawDescriptor(model));
        model.ticket = Ticket{[id = app.drawCount, app = &app](){
            app->draws.erase(id);
        }};
        app.drawCount++;
    }

    namespace detail {
        static constexpr size_t framesInFlight = 2;
    }
}

#endif //PIPERINE_APPLICATION_H
