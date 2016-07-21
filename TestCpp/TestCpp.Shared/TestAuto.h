#pragma once
class TestAuto
{
public:
    TestAuto();
    ~TestAuto();

    static const TestAuto& getObject();
    static void autoReference();
    static void main();
};

