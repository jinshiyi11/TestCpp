#pragma once
#include "Set.h"
class TestEasySet
{
public:
    TestEasySet();
    ~TestEasySet();

    static void main();

private:
    template<typename T>
    static void print(const T& t);
};