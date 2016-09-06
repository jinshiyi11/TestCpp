#include "TestEasySet.h"
#include <iostream>
#include "Set.h"
#include <string>


TestEasySet::TestEasySet()
{
}


TestEasySet::~TestEasySet()
{
}

template<typename T>
void TestEasySet::print(const T& t)
{
    std::cout << "size:" << t.size() << std::endl;
    for (auto it = t.begin(); it != t.end(); it++) {
        std::cout << *it << std::endl;
    }
}


void TestEasySet::main()
{
    easy::set<std::string> mySet;

    std::cout << "insert aa" << std::endl;
    mySet.insert("aa");
    std::cout << "insert bb" << std::endl;
    mySet.insert("bb");
    print(mySet);

    std::cout << "insert aa" << std::endl;
    auto result=mySet.insert("aa");
    if (result.second) {
        std::cout << "item aa not exist" << std::endl;
    } else {
        std::cout << "item aa exist" << std::endl;
    }

    std::cout << "insert xx" << std::endl;
    result = mySet.insert("xx");
    if (result.second) {
        std::cout << "item xx not exist" << std::endl;
    } else {
        std::cout << "item xx exist" << std::endl;
    }

    std::cout << "erase bb" << std::endl;
    mySet.erase("bb");
    print(mySet);

    std::cout << "clear" << std::endl;
    mySet.clear();
    std::cout << "insert dd" << std::endl;
    mySet.insert("dd");
    print(mySet);

}
