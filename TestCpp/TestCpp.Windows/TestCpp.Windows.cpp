// TestCpp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TestTuple.h"
#include "TestRValueReference.h"
#include "functor/TestBind.h"
#include "TestAuto.h"

#include <functional>


int main()
{
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

