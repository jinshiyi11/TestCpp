#pragma once
class IHello
{
public:
    virtual ~IHello() { }
    virtual void Say() = 0;
};

class CHello
{
public:
    CHello();
    ~CHello();

    virtual void Say();
};

