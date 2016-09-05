#pragma once

/**
 * 测试虚析构函数
 */

class IVirtualDestructor {
public:
    virtual ~IVirtualDestructor();

};

class TestVirtualDestructor:public IVirtualDestructor
{
public:
    TestVirtualDestructor();
    ~TestVirtualDestructor();

    static void main();
};

