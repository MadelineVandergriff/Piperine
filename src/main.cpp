//
// Created by jack on 6/24/21.
//


#include "application.h"
#include "commands.h"
#include "shader_types.h"
#include "mesh.h"


#include <glm/gtx/transform.hpp>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>

using namespace pip;

struct FogUniform {
    glm::vec3 fogColor;
};

FogUniform randUniform() {
    return {{rand(), rand(), rand()}};
}

/*
auto timeSince(auto start) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
}

int main() {
    Interface<InterfaceDescriptor{.shader = {.pushConstantSize = 15}}> feature;
    feature.data[0] = 15;
}*/

int main() {
    const std::vector<Vertex> vertices{
            {.pos = {0.0f, -0.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}},
            {.pos = {0.5f, 0.5f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}},
            {.pos = {-0.5f, 0.5f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}}
    };

    const std::vector<Vertex> rectVertices{
            {.pos = {-0.5f, -0.5f, 0.0f}, .color = {1.0f, 0.0f, 0.0f}},
            {.pos = {0.5f, -0.5f, 0.0f}, .color = {0.0f, 1.0f, 0.0f}},
            {.pos = {0.5f, 0.5f, 0.0f}, .color = {0.0f, 0.0f, 1.0f}},
            {.pos = {-0.5f, 0.5f, 0.0f}, .color = {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> rectIndices{
            0, 1, 2, 2, 3, 0
    };

    Application app{{.resizeable = true},
        PipelineGraph{
            .shaderDescriptors = {
                {0, {"/home/jack/Documents/Piperine/example/shaders/simple.vert.spv", ShaderType::VERTEX, {makeBinding<Vertex>(0)}, sizeof(glm::mat4)}},
                {1, {"/home/jack/Documents/Piperine/example/shaders/simple.frag.spv", ShaderType::FRAGMENT, {}, {}, sizeof(FogUniform)}}
            },
            .pipelineDescriptors = {{0, 1, {}}}
        }, [](Application& app){
        if (app.actualFrame % 144 == 0) {
            std::cout << "Tick\n";
        }
    }};

    //auto triangle = createMesh(app.device, vertices);
    //app.draws.push_back(getDrawDescriptor(triangle));

    glm::vec3 camPos = { 0.f,0.f,-3.f };

    //glm::mat4 view = glm::translate(glm::mat4(1.f), camPos);
    glm::mat4 view = glm::lookAt(camPos, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
    //camera projection
    glm::mat4 projection = glm::perspective(glm::radians(70.f), 800.f / 600.f, 0.1f, 200.0f);
    projection[1][1] *= -1;
    //model rotation
    //glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(app.actualFrame * 0.4f), glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4{1.f};

    //calculate final mesh matrix
    glm::mat4 mesh_matrix = projection * view * model;

    Model<MVPConstant> rectangle{createMesh(app.device, rectVertices, rectIndices), {mesh_matrix}};
    registerModel(app, rectangle);

    FogUniform fog{{0.2f, 0.2f, 0.2f}};
    app.uniformDescriptors.push_back(GenericData{&fog, sizeof(fog)});

    bool q_pressed{false};

    while (!glfwWindowShouldClose(*app.window.window)) {
        glfwPollEvents();
        drawFrame(app);
        //model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(app.actualFrame * 0.4f), glm::vec3(0, 1, 0));
        view = glm::lookAt(camPos, glm::vec3{0.f, 0.f, 0.f}, glm::vec3{0.f, 1.f, 0.f});
        rectangle.push = {projection * view * model};

        if (glfwGetKey(*app.window.window, GLFW_KEY_D) == GLFW_PRESS) {
            model = glm::rotate(0.01f, glm::vec3{0.f, 1.f, 0.f}) * model;
        }
        if (glfwGetKey(*app.window.window, GLFW_KEY_A) == GLFW_PRESS) {
            model = glm::rotate(-0.01f, glm::vec3{0.f, 1.f, 0.f}) * model;
        }
        if (glfwGetKey(*app.window.window, GLFW_KEY_S) == GLFW_PRESS) {
            model = glm::rotate(0.01f, glm::vec3{1.f, 0.f, 0.f}) * model;
        }
        if (glfwGetKey(*app.window.window, GLFW_KEY_W) == GLFW_PRESS) {
            model = glm::rotate(-0.01f, glm::vec3{1.f, 0.f, 0.f}) * model;
        }
        if (glfwGetKey(*app.window.window, GLFW_KEY_Q) == GLFW_PRESS) {
            if (!q_pressed) {
                q_pressed = true;
                fog.fogColor.x = ((int) (fog.fogColor.x * 45678) % 100 + 1) / 100.f;
                fog.fogColor.y = ((int) (fog.fogColor.x * 93612) % 100 + 1) / 100.f;
                fog.fogColor.z = ((int) (fog.fogColor.x * 18420) % 100 + 1) / 100.f;
                app.uniformDescriptors.push_back(GenericData{&fog, sizeof(fog)});
                std::cout << "Fog color updated\n";
            }
        } else {
            q_pressed = false;
        }
    }

    vkDeviceWaitIdle(*app.device.logical);

    return 0;
}