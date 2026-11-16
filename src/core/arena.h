#ifndef VESPERA_CORE_ARENA_H
#define VESPERA_CORE_ARENA_H

#include <span>
#include <cstddef>

namespace vespera::core {
    class Arena {
    public:
        explicit Arena(std::span<std::byte> memory);

        Arena(const Arena&) = delete;
        Arena& operator=(const Arena&) = delete;

        Arena(Arena&&) = delete;
        Arena& operator=(Arena&&) = delete;

        // returns nullptr if the allocation fails
        std::byte* try_allocate(
            std::size_t num_bytes,
            std::size_t alignment = std::size_t{alignof(std::max_align_t)}
        );

        void reset();

        template <typename T, typename... Args>
        requires
            std::is_trivially_destructible_v<T>
        T* create(Args&&... args);

        template <typename T>
        requires
            std::is_trivially_constructible_v<T> &&
            std::is_trivially_destructible_v<T>
        std::span<T> create_array(std::size_t count);

    private:
        std::byte* m_Begin   = nullptr;
        std::byte* m_End     = nullptr;
        std::byte* m_Current = nullptr;
    };
}

#include "arena.inl"

#endif
