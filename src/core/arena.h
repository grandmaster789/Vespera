#ifndef VESPERA_CORE_ARENA_H
#define VESPERA_CORE_ARENA_H

#include <cstddef>
#include <span>
#include <type_traits>

namespace vespera::core {
    // Bump allocator over a caller-owned buffer. The arena does not own the
    // buffer; it must outlive the arena (and must not reallocate, e.g. a growing std::vector).
    // Construct from any contiguous container with create_arena(buffer).
    class Arena {
    public:
        explicit Arena(std::span<std::byte> buffer) noexcept;

        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;

        Arena(Arena&&) = delete;
        Arena& operator=(Arena&&) = delete;

        // returns nullptr if the allocation fails
        std::byte* try_allocate(
            std::size_t num_bytes,
            std::size_t alignment = std::size_t{alignof(std::max_align_t)}
        ) noexcept;

        void reset() noexcept;
        [[nodiscard]] std::size_t capacity() const noexcept;
        [[nodiscard]] std::size_t bytes_available() const noexcept;

        template <typename T, typename... Args>
        requires
            std::is_trivially_destructible_v<T>
        T* create(Args&&... args);

        // returns an empty span if the allocation fails
        template <typename T>
        requires
            std::is_default_constructible_v<T> &&
            std::is_trivially_destructible_v<T>
        std::span<T> create_array(std::size_t count);

    private:
        std::byte* m_Begin   = nullptr;
        std::byte* m_End     = nullptr;
        std::byte* m_Current = nullptr;
    };

    // Takes an lvalue so temporaries can't be used as backing storage.
    template <typename Buffer>
    requires requires (Buffer& b) { std::as_writable_bytes(std::span(b)); }
    Arena create_arena(Buffer& buffer) noexcept {
        return Arena(std::as_writable_bytes(std::span(buffer)));
    }
}

#include "arena.inl"

#endif
