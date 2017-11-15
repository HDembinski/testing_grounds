#include <iostream>

static unsigned ctor_count = 0;

struct A {
    A() { ++ctor_count; }
    A(const A&) { ++ctor_count; }
    A& operator=(const A&) { ++ctor_count; return *this; }
    A(A&&) {}
    A& operator=(A&&) { return *this; }
    A& operator+=(const A&) { return *this; }
};

// https://stackoverflow.com/questions/15597754/move-semantics-to-avoid-temporary-object-creation

namespace v1 {
    A&& operator+(A &&a, const A &b)
    { a+=b; return std::move(a); }

    A&& operator+(A &&a, A &&b)
    { a+=b; return std::move(a); }

    A&& operator+(const A &a, A &&b)
    { b+=a; return std::move(b); }

    A operator+(const A &a, const A &b)
    { A r(a); r+=b; return r; }
}

namespace v2 {
    A operator+(A &&a, A &&b)
    { a+=b; return a; }

    A operator+(A &&a, const A &b)
    { a+=b; return a; }

    A operator+(const A &a, A &&b)
    { return operator+(std::move(b), a); }

    A operator+(const A &a, const A &b)
    { A r(a); r+=b; return r; }
}

namespace v3 {
    A operator+(const A &a, const A &b)
    { A r(a); r+=b; return r; }
}

int main() {
    {
        using namespace v1;
        std::cout << "v1" << std::endl;
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

    {
        using namespace v2;
        std::cout << "v2" << std::endl;
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

    {
        using namespace v3;
        std::cout << "v3" << std::endl;
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
}