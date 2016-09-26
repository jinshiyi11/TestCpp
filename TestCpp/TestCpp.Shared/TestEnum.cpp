#include "TestEnum.h"
#include <iostream>


namespace test {
    TestEnum::TestEnum()
    {
    }


    TestEnum::~TestEnum()
    {
    }

    void test::TestEnum::print(Color color)
    {
        std::cout << color;
    }
}
