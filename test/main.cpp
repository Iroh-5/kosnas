#include <gtest/gtest.h>

namespace nas::crypto::test {

void test();

} // namespace nas::crypto::test

int main(int argc, char** argv)
{
    nas::crypto::test::test();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
