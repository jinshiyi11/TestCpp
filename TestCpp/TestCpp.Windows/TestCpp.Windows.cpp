// TestCpp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "TestTuple.h"
#include "TestRValueReference.h"
#include "functor/TestBind.h"

#include <functional>


int main()
{
    //TestTuple::test();

    TestBind::test();

    std::function<void() > func;
    if (func)
        func();

    TestRValueReference::test();
    return 0;
}

