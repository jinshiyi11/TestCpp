// TestCpp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TestTuple.h"
#include "TestRValueReference.h"
#include "functor/TestBind.h"
#include "TestAuto.h"
#include "TestInterface.h"
#include "TestVirtualDestructor.h"

#include <functional>
#include "EASTL/map.h"
#include <memory>
#include <string>
#include <map>
#include "TestEasyMap.h"



int main()
{
    //std::make_pair(1, 2);

    //std::map<int, int> map1;
    //map1.insert(std::make_pair(1, 2));

    //eastl::pair
    eastl::map<int,int> map2;
    map2.insert(eastl::make_pair(2,3));

    TestVirtualDestructor::main();
    std::u16string us1 = u"aaa";
    std::u16string us2 = u"bbb";
    if (us1 == us2) {

    }


    std::unique_ptr<int> p1;
    if (p1 == nullptr) {

    }
    eastl::map<int,int> m1;

    {
        int size = sizeof(IHello);
        CHello hello;
    }
    int x = 0;
    int y = 1;

    int& p = x;
    p = y;

    //TestTuple::test();
    TestAuto::main();

    TestBind::test();

    std::function<void() > func;
    if (func)
        func();

    TestRValueReference::test();
    return 0;
}

