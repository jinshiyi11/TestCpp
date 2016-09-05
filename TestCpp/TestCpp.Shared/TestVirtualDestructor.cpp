#include "TestVirtualDestructor.h"
#include <iostream>


IVirtualDestructor::~IVirtualDestructor()
{
    std::cout << "~IVirtualDestructor()"<<std::endl;
}


TestVirtualDestructor::TestVirtualDestructor()
{
}


TestVirtualDestructor::~TestVirtualDestructor()
{
    std::cout << "~TestVirtualDestructor()" << std::endl;
}

void TestVirtualDestructor::main()
{
    IVirtualDestructor* pObj = new TestVirtualDestructor();
    delete pObj;

    std::cout << "TestVirtualDestructor::main() end" << std::endl;
}
