#include "core/arena.h"
#include "core/assert.h"

#include <bit>
#include <cstdint>

namespace vespera::core {
    Arena::Arena(std::span<std::byte> buffer):
        m_Begin  (buffer.data()),
        m_End    (buffer.data() + buffer.size()),
        m_Current(buffer.data())
    {
    }

    std::byte* Arena::try_allocate(
        std::size_t num_bytes,
        std::size_t alignment
    ) {
        VESPERA_ASSERT(std::has_single_bit(alignment), "Alignment must be a power of 2");

        const auto           p       = reinterpret_cast<std::uintptr_t>(m_Current);
        const std::uintptr_t aligned = (p + alignment - 1) & ~(alignment - 1);
        const std::size_t    padding = aligned - p;

        if (const auto remaining = static_cast<const std::size_t>(m_End - m_Current);
            padding > remaining ||
            num_bytes > remaining - padding
        )
            return nullptr;

        m_Current += padding + num_bytes;

        return m_Current - num_bytes;
    }

    void Arena::reset() {
        m_Current = m_Begin;
    }
}