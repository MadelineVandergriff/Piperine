//
// Created by jack on 8/12/21.
//

#ifndef PIPERINE_PUBLIC_UTILITY_H
#define PIPERINE_PUBLIC_UTILITY_H

#include "definitions.h"

#include <memory>
#include <set>
#include <cassert>
#include <utility>

namespace pip::detail {
    using erased_ptr = std::unique_ptr<void, void (*)(void *)>;

    void null_deleter(void *);
    erased_ptr getEmptyErasedPtr();

    template<typename T, typename... Args>
    erased_ptr make_erased(Args&&... args) {
        struct Deleter {
            static void deleter(void* ptr) {
                delete static_cast<T*>(ptr);
            }
        };
        return erased_ptr{static_cast<void*>(new T{std::forward<Args>(args)...}), Deleter::deleter};
    }

    template<typename T, typename... Args>
    erased_ptr append_erased(erased_ptr&& old, Args&&... args) {
        struct erased_ptr_chain {
            T value;
            erased_ptr next;
        };

        struct Deleter {
            static void deleter(void* ptr) {
                delete static_cast<erased_ptr_chain*>(ptr);
            }
        };
        return erased_ptr{static_cast<void*>(new erased_ptr_chain{{std::forward<Args>(args)...}, std::move(old)}), Deleter::deleter};
    }

    constexpr int strcmp_cx(const char* s1, const char* s2)
    {
        while (*s1 && (*s1 == *s2))
            s1++, s2++;
        return *(const unsigned char*)s1 - *(const unsigned char*)s2;
    }

    template<typename T, T... ts>
    struct ArrayPack {};

    template<typename T, size_t size, std::array<T, size> array, size_t... I>
    constexpr auto pack_array_helper(std::index_sequence<I...>) {
        return ArrayPack<T, array.at(I)...>{};
    }

    template<typename T, size_t size, std::array<T, size> array>
    using pack_array = decltype(pack_array_helper<T, size, array>(std::make_index_sequence<size>{}));

    template<typename T>
    struct Wrap {
        Wrap() = default;
        Wrap(T t) : internal{t} {}
        Wrap(void* ptr) requires (!std::is_same_v<T, std::remove_pointer_t<T>>) : internal{static_cast<T>(ptr)} {}

        operator T() const {
            return internal;
        }
    private:
        T internal{};
    };

    template<typename T>
    class Ref {
    public:
        T* operator->() const {
            assert(ptr != nullptr);
            return static_cast<T*>(ptr.get());
        }

        T& operator*() const {
            assert(ptr != nullptr);
            return *static_cast<T*>(ptr.get());
        }

        void clear() {
            ptr = nullptr;
        }
    private:
        ErasedRef ptr{nullptr};
    };


    template<typename T>
    class PtrRef {
    public:
        T operator*() const {
            assert(ptr != nullptr);
            return T{ptr.get()};
        }

        void clear() {
            ptr = nullptr;
        }

        PtrRef() = default;
        PtrRef(void* ptr, auto destructor) : ptr{ptr, destructor} {}
    private:
        ErasedRef ptr{nullptr};
    };

    template<typename T>
    class Set {
    public:
        using container = std::set<T>;
        using iterator = typename container::iterator;
        using const_iterator = typename container::const_iterator;
        using size_type = typename container::size_type;
        using key_type = typename container::key_type;

        template<typename... Args>
        std::pair<iterator, bool> emplace( Args&&... args ) {
            return internal.template emplace(std::forward<Args>(args)...);
        }

        iterator erase(iterator pos) { return internal.erase(pos); }
        iterator erase(const_iterator pos) { return internal.erase(pos); }
        iterator erase(const_iterator first, const_iterator last) { return internal.erase(first, last); }
        size_type erase(const key_type& key) { return internal.erase(key); }
    private:
        container internal;
    };
}
#endif //PIPERINE_PUBLIC_UTILITY_H
