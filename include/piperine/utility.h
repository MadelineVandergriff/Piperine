//
// Created by jack on 6/16/21.
//

#ifndef PIPERINE_UTILITY_H
#define PIPERINE_UTILITY_H

#include "piperine/definitions.h"
#include "piperine/interface.h"
#include "piperine/public_utility.h"

#include <iostream>
#include <cassert>
#include <memory>
#include <filesystem>
#include <cstddef>
#include <utility>
#include <functional>

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

    enum class PIPERINE_API LogType {
        CREATION, DESTRUCTION, VALIDATION, BASIC
    };

    enum class PIPERINE_API LogSeverity {
        ERROR, WARNING, LOG
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

    void PIPERINE_API log(const char* msg, LogType type = LogType::BASIC, LogSeverity severity = LogSeverity::LOG);

    template<typename T>
    struct Singleton {
        static const T& getSingleton() {
            static const T singleton{};
            return singleton;
        }
    };
}

#endif //PIPERINE_UTILITY_H
