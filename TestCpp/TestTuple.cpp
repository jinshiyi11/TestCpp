#include "TestTuple.h"
#include <tuple>
#include <string>
#include <iostream>


TestTuple::TestTuple()
{
}


TestTuple::~TestTuple()
{
}

void TestTuple::test()
{
    std::tuple<int, std::string, float> t1(1, "ggd", 5.1);

    int x = std::get<0>(t1);

    std::cout << "testTuple ok";
}
