#include "TestRValueReference.h"

Student::Student() :mId(0), mAge(0)
{

}

Student::Student(const Student& other)
{
    mId = other.mId;
    mAge = other.mId;
    mName = other.mName;
}

void Student::setId(int id)
{
    mId = id;
}

void Student::setAge(int age)
{
    mAge = age;
}

void Student::setName(const std::string& name)
{
    mName = name;
}

TestRValueReference::TestRValueReference()
{
}


TestRValueReference::~TestRValueReference()
{
}

void TestRValueReference::test()
{
    TestRValueReference test;
    Student student=test.getStudent(1);

}

Student TestRValueReference::getStudent(int id)
{
    Student student;
    student.setId(id);
    return student;
}

