#include <clean-core/container/vector.hh>
#include <typed-geometry/linalg/vec.hh>
#include <core/assert.h>

int main() {
    cc::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    VESPERA_CHECK(v.size() == 2, "cc::vector must grow");

    tg::vec3f a(1, 2, 3);
    tg::vec3f b(4, 5, 6);
    VESPERA_CHECK((a + b)[0] == 5, "tg vector addition");
}
