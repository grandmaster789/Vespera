#ifndef VESPERA_CORE_ARENA_H
#define VESPERA_CORE_ARENA_H

#include <span>
#include <cstddef>
#include <array>
#include <vector>

namespace vespera::core {
    class Arena {
    public:
        explicit Arena(std::span<std::byte> buffer);

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
        std::size_t get_capacity() const noexcept;
        std::size_t bytes_available() const noexcept;

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

    template <typename T, size_t N>
    Arena create_arena(std::array<T, N>& buffer);

    template <typename T>
    Arena create_arena(std::vector<T>& buffer);
}

#include "arena.inl"

#endif
