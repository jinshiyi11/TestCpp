#include "TestEasyMap.h"
#include <iostream>


TestEasyMap::TestEasyMap()
{
}


TestEasyMap::~TestEasyMap()
{
}

template<typename T>
void TestEasyMap::print(const T& t)
{
    std::cout << "size:" << t.size() << std::endl;
    for (auto it = t.begin(); it != t.end(); it++) {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

void TestEasyMap::main()
{
    easy::map<int, int> myMap;

    myMap.insert(easy::make_pair(1, 2));
    myMap.insert(easy::make_pair(3, 4));
    myMap.insert(easy::make_pair(5, 6));
    print(myMap);

    myMap.erase(3);
    myMap.insert(easy::make_pair(5, 11));
    print(myMap);

    auto it = myMap.find(7);
    if (it == myMap.end()) {
        std::cout << "key 7 not exist" << std::endl;
    } else {
        std::cout << "key 7 exist" << std::endl;
    }

    it = myMap.find(5);
    if (it == myMap.end()) {
        std::cout << "key 5 not exist" << std::endl;
    } else {
        std::cout << "key 5 exist" << std::endl;
    }

    myMap.clear();
    print(myMap);
    

}
