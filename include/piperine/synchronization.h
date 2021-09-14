//
// Created by jack on 7/15/21.
//

#ifndef PIPERINE_SYNCHRONIZATION_H
#define PIPERINE_SYNCHRONIZATION_H

#include <array>
#include <atomic>
#include <type_traits>
#include <cstdint>
#include <optional>
#include <memory>
#include <utility>

namespace pip {
    namespace detail {
        template<size_t Size>
        using fastest_unsigned_integer =
            std::conditional_t<(UINT_FAST8_MAX > Size), uint_fast8_t,
            std::conditional_t<(UINT_FAST16_MAX > Size), uint_fast16_t,
            std::conditional_t<(UINT_FAST32_MAX > Size), uint_fast32_t, uint_fast64_t>>>;

        template<typename T, size_t Size, bool UseAtomics = true>
        struct RingBufferBase {
            std::array<T, Size> buffer;
            std::conditional_t<UseAtomics, std::atomic_size_t, size_t> read{0};
            std::conditional_t<UseAtomics, std::atomic_size_t, size_t> write{0};
        };
    }

    template<typename T, size_t Size, bool UseAtomics = true>
    class RingBufferWriter;

    template<typename T, size_t Size, bool UseAtomics = true>
    class RingBufferReader {
    public:
        std::optional<T> attemptRead() {
            if (ready()) {
                return readUnsafe();
            }
            return {};
        }

        T readUnsafe() {
            auto result = underlying->buffer.at(underlying->read);
            underlying->read = (underlying->read + 1) % Size;
            return result;
        }

        T reread() {
            return underlying->buffer.at(underlying->read);
        }

        [[nodiscard]] bool ready() const {
            return underlying->read != underlying->write;
        }

        RingBufferReader()=delete;
        RingBufferReader(const RingBufferReader&)=delete;
        RingBufferReader& operator=(const RingBufferReader&)=delete;
        RingBufferReader(RingBufferReader&&) noexcept =default;
        RingBufferReader& operator=(RingBufferReader&&) noexcept =default;

        template<typename _T, size_t _Size, bool _UseAtomics>
        friend std::pair<RingBufferReader<_T, _Size, _UseAtomics>, RingBufferWriter<_T, _Size, _UseAtomics>> createRingBuffer();
    private:
        explicit RingBufferReader(const std::shared_ptr<detail::RingBufferBase<T, Size, UseAtomics>>& underlying) : underlying{underlying} {}
        std::shared_ptr<detail::RingBufferBase<T, Size, UseAtomics>> underlying;
    };

    template<typename T, size_t Size, bool UseAtomics>
    class RingBufferWriter {
    public:
        bool attemptWrite(T in) {
            if (ready()) {
                writeUnsafe(std::move(in));
                return true;
            }
            return false;
        }

        void writeUnsafe(T in) {
            underlying->buffer.at(underlying->write) = std::move(in);
            underlying->write = (underlying->write + 1) % Size;
        }

        void rewrite(T in) {
            underlying->buffer.at(underlying->write) = std::move(in);
        }

        [[nodiscard]] bool alreadyAdvanced() const {
            return underlying->read != underlying->write;
        }

        [[nodiscard]] bool ready() const {
            return (underlying->write + 1) % Size != underlying->read;
        }

        RingBufferWriter()=delete;
        RingBufferWriter(const RingBufferWriter&)=delete;
        RingBufferWriter& operator=(const RingBufferWriter&)=delete;
        RingBufferWriter(RingBufferWriter&&) noexcept =default;
        RingBufferWriter& operator=(RingBufferWriter&&) noexcept =default;

        template<typename _T, size_t _Size, bool _UseAtomics>
        friend std::pair<RingBufferReader<_T, _Size, _UseAtomics>, RingBufferWriter<_T, _Size, _UseAtomics>> createRingBuffer();
    private:
        explicit RingBufferWriter(const std::shared_ptr<detail::RingBufferBase<T, Size, UseAtomics>>& underlying) : underlying{underlying} {}
        std::shared_ptr<detail::RingBufferBase<T, Size, UseAtomics>> underlying;
    };

    template<typename T, size_t Size, bool UseAtomics = true>
    std::pair<RingBufferReader<T, Size, UseAtomics>, RingBufferWriter<T, Size, UseAtomics>> createRingBuffer() {
        auto underlying = std::make_shared<detail::RingBufferBase<T, Size, UseAtomics>>();
        return {RingBufferReader<T, Size, UseAtomics>{underlying}, RingBufferWriter<T, Size, UseAtomics>{underlying}};
    }
}

#endif //PIPERINE_SYNCHRONIZATION_H
