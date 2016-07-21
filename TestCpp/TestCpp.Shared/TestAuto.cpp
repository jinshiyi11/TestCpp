#include "TestAuto.h"
#include <iostream>
#include <typeinfo>

TestAuto::TestAuto()
{
}


TestAuto::~TestAuto()
{
}

void TestAuto::main()
{
    for (auto& z : {5.1,6.2}) {
        std::cout << z << std::endl;
    }

    float&& y = 1;
    float i = 0;
    auto a1 = i;
    auto& a2 = i;
    const auto& a3 = i;

    auto&& a4 = i;
    std::cout << typeid(a1).name() << std::endl;
    std::cout << typeid(a2).name() << std::endl;
    std::cout << typeid(a3).name() << std::endl;
    std::cout << typeid(a4).name() << std::endl;
    std::cout << std::endl;

}
