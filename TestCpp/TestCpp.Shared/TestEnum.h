#ifndef __TESTENUM_H__
#define __TESTENUM_H__

namespace test {
    class TestEnum
    {
    public:
        enum Color {
            Green,
            Red
        };

        TestEnum();
        ~TestEnum();

        void print(Color color);
    };
}
#endif // __TESTENUM_H__
