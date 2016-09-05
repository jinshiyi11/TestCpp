#include "TestEasyMap.h"
#include "Map.h"


TestEasyMap::TestEasyMap()
{
}


TestEasyMap::~TestEasyMap()
{
}

void TestEasyMap::main()
{
    easy::map<int, int> myMap;

    myMap.insert(easy::make_pair(1, 2));

}
