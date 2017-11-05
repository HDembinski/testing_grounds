#include <iostream>

static unsigned ctor_count = 0;

struct A {
    A() { ++ctor_count; }
    A(const A&) { ++ctor_count; }
    A& operator=(const A&) { return *this; }
    A(A&&) {}
    A& operator=(A&&) { return *this; }
    A& operator+=(const A&) { return *this; }
};

// https://stackoverflow.com/questions/15597754/move-semantics-to-avoid-temporary-object-creation

A&& operator+(A &&a, const A &b)
{ a+=b; return std::move(a); }

A&& operator+(A &&a, A &&b)
{ a+=b; return std::move(a); }

A&& operator+(const A &a, A &&b)
{ b+=a; return std::move(b); }

A operator+(const A &a, const A &b)
{ A r(a); r+=b; return r; }

int main() {
    A a, b, c, d;
    ctor_count = 0;
    A r1 = a + b + c + d;
    std::cout << "1: " << ctor_count << std::endl;
    ctor_count = 0;
    A r2 = (a + b) + c + d;
    std::cout << "2: " << ctor_count << std::endl;
    ctor_count = 0;
    A r3 = a + (b + c) + d;
    std::cout << "3: " << ctor_count << std::endl;
    ctor_count = 0;
    A r4 = (a + b) + (c + d);
    std::cout << "4: " << ctor_count << std::endl;
}