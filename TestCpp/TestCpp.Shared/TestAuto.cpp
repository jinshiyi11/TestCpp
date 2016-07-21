#include "TestAuto.h"
#include <iostream>
#include <typeinfo>

TestAuto::TestAuto()
{
}


TestAuto::~TestAuto()
{
}

const TestAuto& TestAuto::getObject()
{
    static TestAuto obj;
    return obj;
}

void TestAuto::autoReference()
{
    /**
     * Note that using auto drops references, const qualifiers, and volatile qualifiers. 
     * https://msdn.microsoft.com/en-us/library/dd293667.aspx
     * You may believe myAuto is an int reference, but it is not.
     * It is simply an int, so the output is 11 11, not 11 12 as would be the case if the reference had not been dropped by auto.
     */
    int count = 10;
    int& countRef = count;
    auto myAuto = countRef;

    countRef = 11;
    std::cout << count << " ";

    myAuto = 12;
    std::cout << count << std::endl;
}

void TestAuto::main()
{
    auto obj1 = getObject();
    auto& obj2 = getObject();
    const auto& obj3 = getObject();

    autoReference();
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
