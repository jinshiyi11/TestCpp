#pragma once
#include "Map.h"
class TestEasyMap
{
public:
    TestEasyMap();
    ~TestEasyMap();

    static void main();

private:
    template<typename T>
    static void print(const T& t);
};

