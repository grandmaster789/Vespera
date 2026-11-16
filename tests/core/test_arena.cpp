#include <core/arena.h>
#include <core/assert.h>

#include <array>
#include <cstdint>
#include <span>
#include <vector>

using vespera::core::Arena;

struct Foo {
    int i = 123;
    float f = 3.14f;
};

struct Baz {
    bool b = true;
};

struct alignas(16) Aligned16 {
    char c;
};

int main() {
    // create<T> constructs a usable object
    std::array<unsigned char, 1024> buffer = {};
    auto arena = vespera::core::create_arena(buffer);

    VESPERA_CHECK(arena.capacity() == sizeof(buffer), "capacity must equal buffer size in bytes");

    Foo* foo = arena.create<Foo>();
    VESPERA_CHECK(foo, "Expected allocation to succeed");
    VESPERA_CHECK(foo->i == 123 && foo->f == 3.14f, "Expected default member initializers to run");
    foo->i = 456;
    foo->f = 2.718f;
    VESPERA_CHECK(foo->i == 456, "Expected i == 456");
    VESPERA_CHECK(foo->f == 2.718f, "Expected f == 2.718");

    // capacity is measured in bytes for non-byte element types
    std::vector<Foo> foo_buffer(256);
    auto arena2 = vespera::core::create_arena(foo_buffer);

    VESPERA_CHECK(arena2.capacity() == foo_buffer.size() * sizeof(Foo), "capacity must be in bytes, not elements");

    auto* bar = arena2.create<Foo>(789, 3.33f);
    VESPERA_CHECK(bar->i == 789, "Expected i == 789");
    VESPERA_CHECK(bar->f == 3.33f, "Expected f == 3.33");

    auto* baz = arena2.create<Baz>();
    VESPERA_CHECK(baz->b, "Expected baz->b == true");

    // create_array value-initializes and returns the start of the array
    arena.reset();
    VESPERA_CHECK(arena.bytes_available() == arena.capacity(), "reset must free everything");

    auto ints = arena.create_array<int>(4);
    VESPERA_CHECK(ints.size() == 4, "Expected 4 ints");
    for (int x : ints)
        VESPERA_CHECK(x == 0, "Expected value-initialized ints");

    auto foos = arena.create_array<Foo>(3);
    VESPERA_CHECK(foos.size() == 3, "Expected 3 Foos");
    for (const Foo& f : foos)
        VESPERA_CHECK(f.i == 123, "Expected default-constructed Foos");

    // alignment is honoured after an odd-sized allocation
    arena.reset();
    arena.try_allocate(1, 1);
    auto* aligned = arena.create<Aligned16>();
    VESPERA_CHECK(reinterpret_cast<std::uintptr_t>(aligned) % 16 == 0, "Expected 16-byte alignment");

    // exhaustion returns nullptr / empty span and never underflows
    arena.reset();
    while (arena.try_allocate(100, 1)) {}
    VESPERA_CHECK(arena.bytes_available() < 100, "Expected arena to be nearly full");
    VESPERA_CHECK(arena.try_allocate(arena.bytes_available() + 1, 1) == nullptr, "Expected over-allocation to fail");
    VESPERA_CHECK(arena.try_allocate(arena.bytes_available(), 1) != nullptr, "Expected exact fit to succeed");
    VESPERA_CHECK(arena.bytes_available() == 0, "Expected arena to be full");
    VESPERA_CHECK(arena.create<Foo>() == nullptr, "Expected create to fail when full");
    VESPERA_CHECK(arena.create_array<int>(1).empty(), "Expected create_array to fail when full");
}
