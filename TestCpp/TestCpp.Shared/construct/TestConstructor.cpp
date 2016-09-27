#include "TestConstructor.h"
#include <iostream>

TestConstructor::TestConstructor()
    :mX(0),
    mName("hehe")
{
}

TestConstructor::TestConstructor(const TestConstructor& other)
{
    //如果实现了拷贝构造函数，但是什么都不做会怎么样
    //1.该类有vptr，2.该类无vptr

}

TestConstructor::~TestConstructor()
{
}

void TestConstructor::setX(int x)
{
    mX = x;
}

int TestConstructor::getX() const
{
    return mX;
}

void TestConstructor::main()
{
    TestConstructor a1;
    a1.setX(7);

    TestConstructor a2 = a1;

    std::cout << std::endl;
}
