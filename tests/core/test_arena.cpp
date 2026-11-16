#include <core/arena.h>

#include <array>
#include <vector>
#include <span>

struct Foo {
    int i = 123;
    float f = 3.14f;
};

int main() {
    std::array<unsigned char, 1024> buffer = {};
    auto arena = vespera::core::create_arena(buffer);

    Foo* foo = arena.create<Foo>();
    foo->i = 456;
    foo->f = 2.718f;
}