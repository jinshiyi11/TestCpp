#pragma once
#include <string>
class TestConstructor
{
public:
    TestConstructor();

    TestConstructor(const TestConstructor& other);

    virtual ~TestConstructor();
    void setX(int x);
    int getX() const;

    static void main();
private:
    int mX;
    std::string mName;
};

