//
// Created by jack on 6/16/21.
//

#ifndef PIPERINE_UTILITY_H
#define PIPERINE_UTILITY_H

#include <iostream>
#include <cassert>
#include <memory>
#include <filesystem>
#include <cstddef>
#include <utility>

#include <vulkan/vulkan.hpp>

namespace pip {
    template<typename T>
    class Ref {
    public:
        T* operator->() const {
            assert(underlying != nullptr);
            return &*underlying;
        }

        T& operator*() const {
            assert(underlying != nullptr);
            return *underlying;
        }

        void clear() {
            underlying = nullptr;
        }
    protected:
        std::shared_ptr<T> underlying{nullptr};
    };

    template<typename T>
    class PtrRef {
    private:
        using Base = std::remove_const_t<std::remove_pointer_t<T>>;
    public:
        Base* operator*() const {
            assert(underlying != nullptr);
            return &*underlying;
        }

        void clear() {
            underlying = nullptr;
        }
    protected:
        std::shared_ptr<Base> underlying{nullptr};
    };

    struct WindowInfo {
        uint32_t width{800};
        uint32_t height{600};
        const char* title{"Piperine Application"};
        bool resizeable{false};
    };

    struct SwapchainSupportInfo {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
        bool includes8BitSRGBNonlinear{false};
        bool includesMailbox{false};
    };

    enum class ShaderType {
        FRAGMENT, VERTEX, COMPUTE, UNKNOWN
    };

    enum class LogType {
        CREATION, DESTRUCTION, VALIDATION, BASIC
    };

    enum class LogSeverity {
        ERROR, WARNING, LOG
    };

    enum Flags {
        FLAG_FAILURE = 1,
        FLAG_FRAME_INVALIDATED = 1 << 1
    };

    class Ticket {
    public:
        explicit Ticket(std::function<void(void)> destructor) : destructor{std::move(destructor)} {};
        Ticket() = default;
        Ticket(const Ticket&) = delete;
        Ticket& operator=(const Ticket&) = delete;
        ~Ticket() {destructor();};

        Ticket& operator=(Ticket&& other) noexcept {
            destructor = other.destructor;
            other.destructor = [](){};
            return *this;
        }

        Ticket(Ticket&& other) noexcept {
            destructor = other.destructor;
            other.destructor = [](){};
        }
    private:
        std::function<void(void)> destructor;
    };

    void log(const char* msg, LogType type = LogType::BASIC, LogSeverity severity = LogSeverity::LOG);
    std::vector<char> readFile(const std::filesystem::path& path);
}

#endif //PIPERINE_UTILITY_H
