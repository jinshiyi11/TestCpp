#include "TestBind.h"
#include <functional>

using namespace std::placeholders;

TestBind::TestBind()
{
}

TestBind::~TestBind()
{
}

void TestBind::request(Request* request, int id, std::string name)
{

}

void TestBind::print(int id, std::string name, const char* data, size_t dataSize)
{

}

void TestBind::hello()
{
    Request* request = new Request();
    request->setName("hello");
    std::function< void(int id, std::string name) > func = std::bind(&TestBind::request, this, request, _1, _2);
    func(10, "tom");

    auto func1 = std::bind(&TestBind::print, this, 100, _1,_2,_3);
    func1("tom","beijing",10); 
}

void TestBind::test()
{
    TestBind test;
    test.hello();

    auto func1 = std::bind(&TestBind::print, test,100,_1, _2, _3);
    func1("tom", "beijing", 10);

    auto func2 = std::bind(&TestBind::print, &test, 100, _1, _2, _3);
    func2("jim", "beijing", 10);


}
