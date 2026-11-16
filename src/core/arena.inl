#ifndef VESPERA_CORE_ARENA_INL
#define VESPERA_CORE_ARENA_INL

#include <memory>
#include <new>
#include <utility>

#include "arena.h"
#include "assert.h"

namespace vespera::core {
    template <typename T, typename... Args>
    requires
        std::is_trivially_destructible_v<T>
    T* Arena::create(Args&&... args) {
        auto* bytes = try_allocate(sizeof(T), alignof(T));

        if (!bytes)
            return nullptr;

        return new (bytes) T( std::forward<Args>(args)...);
    }

    template <typename T>
    requires
        std::is_trivially_constructible_v<T> &&
        std::is_trivially_destructible_v<T>
    std::span<T> Arena::create_array(std::size_t count) {
        VESPERA_ASSERT(count > SIZE_MAX / sizeof(T), "would overflow");

        auto* bytes = try_allocate(sizeof(T) * count, alignof(T));

        if (!bytes)
            return {};

        T* arr = std::uninitialized_value_construct_n(
            reinterpret_cast<T*>(bytes),
            count
        );

        return {arr, count};
    }

    template <typename T, size_t N>
    Arena create_arena(std::array<T, N>& buffer) {
        auto* start = reinterpret_cast<std::byte*>(buffer.data());

        return Arena({
            start,
            start + buffer.size()
        });
    }

    template <typename T>
    Arena create_arena(std::vector<T>& buffer) {
        auto* start = reinterpret_cast<std::byte*>(buffer.data());

        return Arena({
            start,
            start + buffer.size()}
        );
    }
}

#endif
