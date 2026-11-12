#include <core/logger.h>
#include <string>

struct Foo {
    int i = 123;
    float f = 123.456f;
    bool b = true;
    std::string s = "foo string";
};

int main() {
    Foo foo;

    LOG("hello from logger");
    VESPERA_LOG_INFO("testing 123");
    VESPERA_LOG_WARNING("more {}", 111);
    VESPERA_LOG_TRACE("Foo: i={}, f={}, b={}, s={}", foo.i, foo.f, foo.b, foo.s);
    VESPERA_LOG_ERROR("some error {} {} {}", 2.3, "xxx", true);
    VESPERA_LOG_DEBUG("debugging");

}