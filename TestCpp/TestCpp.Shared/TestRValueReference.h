#pragma once
#include <string>

/**
 * ”“÷µ“˝”√
 */

class Student {
public:
    Student();

    Student(const Student& other);

    void setId(int id);
    void setAge(int age);
    void setName(const std::string& name);

private:
    int mId;
    int mAge;
    std::string mName;
};

class TestRValueReference
{
public:
    TestRValueReference();
    ~TestRValueReference();

    static void test();
private:
    Student getStudent(int id);
};

