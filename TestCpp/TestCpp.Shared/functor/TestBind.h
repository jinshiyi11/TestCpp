#pragma once
#include <string>

class Request {
public:
    void setName(const std::string name) { mName = name; }
private:
    std::string mName;
};

class TestBind
{
public:
    TestBind();
    ~TestBind();

    void print(int id, std::string name,const char* data, size_t dataSize);

    void request(Request* request, int id, std::string name);

    void hello();
    static void test();
};

