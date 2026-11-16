#include <core/arena.h>
#include <core/assert.h>

#include <array>
#include <vector>

struct Foo {
    int i = 123;
    float f = 3.14f;
};

struct Baz {
    bool b = true;
};

int main() {
    std::array<unsigned char, 1024> buffer = {};
    std::vector<Foo> foo_buffer(256);

    auto arena = vespera::core::create_arena(buffer);

    Foo* foo = arena.create<Foo>();
    foo->i = 456;
    foo->f = 2.718f;

    VESPERA_CHECK(foo->i == 456, "Expected i == 456");
    VESPERA_CHECK(foo->f == 2.718f, "Expected f == 2.718");

    auto arena2 = vespera::core::create_arena(foo_buffer);

    auto* bar = arena2.create<Foo>();
    bar->i = 789;
    bar->f = 3.33f;

    VESPERA_CHECK(bar->i == 789, "Expected i == 789");
    VESPERA_CHECK(bar->f == 3.33f, "Expected f == 3.33");

    auto* baz = arena2.create<Baz>();

    VESPERA_CHECK(baz->b, "Expected baz->b == true");
}