#include <clean-core/container/vector.hh>
#include <typed-geometry/linalg/vec.hh>

int main() {
    cc::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    CC_ASSERT_ALWAYS(v.size() == 2, "cc::vector must grow");

    tg::vec3f a(1, 2, 3);
    tg::vec3f b(4, 5, 6);
    CC_ASSERT_ALWAYS((a + b)[0] == 5, "tg vector addition");
}
