#include "core/arena.h"

#include <bit>
#include <cstdint>

#include "clean-core/common/assert.hh"

namespace vespera::core {
    Arena::Arena(std::span<std::byte> buffer) noexcept:
        m_Begin  (buffer.data()),
        m_End    (buffer.data() + buffer.size()),
        m_Current(buffer.data())
    {
    }

    std::byte* Arena::try_allocate(
        std::size_t num_bytes,
        std::size_t alignment
    ) noexcept {
        CC_ASSERT(std::has_single_bit(alignment), "Alignment must be a power of 2");

        const auto           p       = reinterpret_cast<std::uintptr_t>(m_Current);
        const std::uintptr_t aligned = (p + alignment - 1) & ~(alignment - 1);
        const std::size_t    padding = aligned - p;
        const auto           remaining = static_cast<std::size_t>(m_End - m_Current);

        if (padding > remaining || num_bytes > remaining - padding)
            return nullptr;

        std::byte* result = m_Current + padding;
        m_Current = result + num_bytes;

        return result;
    }

    void Arena::reset() noexcept {
        m_Current = m_Begin;
    }

    std::size_t Arena::capacity() const noexcept {
        return static_cast<std::size_t>(m_End - m_Begin);
    }

    std::size_t Arena::bytes_available() const noexcept {
        return static_cast<std::size_t>(m_End - m_Current);
    }
}
