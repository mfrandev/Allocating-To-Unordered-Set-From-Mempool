#ifndef MY_CLASS_H_
#define MY_CLASS_H_

#include <cstdint>

/**
 * Sample POD class definition
 */
class MyClass {

    public:
        uint64_t a;
        uint64_t b;

        // Constructor/default destructor
        MyClass(uint64_t a, uint64_t b) :
        a(a),
        b(b)
        {}
        ~MyClass() = default;

    // Copy constructor/assignment
    MyClass(const MyClass&) = default;
    MyClass& operator=(const MyClass&) = default;

    // Move constructor/assignment
    MyClass(MyClass&&) = default;
    MyClass& operator=(MyClass&&) = default;
};

#endif // MY_CLASS_H_