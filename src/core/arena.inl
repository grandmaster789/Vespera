#ifndef VESPERA_CORE_ARENA_INL
#define VESPERA_CORE_ARENA_INL

#include <cstdint>
#include <memory>
#include <new>
#include <utility>

#include "core/arena.h"
#include "core/assert.h"

namespace vespera::core {
    template <typename T, typename... Args>
    requires
        std::is_trivially_destructible_v<T>
    T* Arena::create(Args&&... args) {
        auto* bytes = try_allocate(sizeof(T), alignof(T));

        if (!bytes)
            return nullptr;

        return new (bytes) T(std::forward<Args>(args)...);
    }

    template <typename T>
    requires
        std::is_default_constructible_v<T> &&
        std::is_trivially_destructible_v<T>
    std::span<T> Arena::create_array(std::size_t count) {
        VESPERA_ASSERT(count <= SIZE_MAX / sizeof(T), "would overflow");

        auto* bytes = try_allocate(sizeof(T) * count, alignof(T));

        if (!bytes)
            return {};

        T* arr = reinterpret_cast<T*>(bytes);
        std::uninitialized_value_construct_n(arr, count);

        return {arr, count};
    }
}

#endif
