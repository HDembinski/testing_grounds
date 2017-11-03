#include <boost/variant.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/int.hpp>
#include <boost/any.hpp>
#include <vector>
#include <algorithm>
#include "benchmark/benchmark.h"

namespace bm = benchmark;
namespace mpl = boost::mpl;
namespace te = boost::type_erasure;

constexpr int nloops = 100;

using c1 = mpl::int_<1>;
using c2 = mpl::int_<2>;

struct virtual_unary {
    virtual ~virtual_unary() {}
    virtual double get(int) const noexcept = 0;
};

struct virtual_unary_val : virtual_unary {
    double val = 1.0;
    virtual double get(int) const noexcept { return val; }
};

struct virtual_unary_arr : virtual_unary {
    std::vector<double> x = std::vector<double>(nloops, 1.0);
    virtual double get(int i) const noexcept { return x[i]; }
};

static void virtual_method_1(bm::State& state) {
    while (state.KeepRunning()) {
        std::unique_ptr<virtual_unary> u(new virtual_unary_val());
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u->get(i));
    }
}

static void virtual_method_2(bm::State& state) {
    while (state.KeepRunning()) {
        std::unique_ptr<virtual_unary> u(new virtual_unary_arr());
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u->get(i));
    }
}

struct variant_unary {
    boost::variant<double, std::vector<double>> var;
    variant_unary(c1) : var(1.0) {}
    variant_unary(c2) : var(std::vector<double>(nloops, 1.0)) {}

    struct getter : boost::static_visitor<double> {
        int idx = 0;
        getter(int x) : idx(x) {}
        double operator()(const double& x) const noexcept {
            return x;
        }
        double operator()(const std::vector<double>& x) const noexcept {
            return x[idx];
        }
    };

    double get(int i) const noexcept {
        return boost::apply_visitor(getter(i), var);
    }
};

static void variant_method_1(bm::State& state) {
    c1 c;
    while (state.KeepRunning()) {
        variant_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

static void variant_method_2(bm::State& state) {
    c2 c;
    while (state.KeepRunning()) {
        variant_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

struct any_unary {
    boost::any var;
    any_unary(c1) : var(1.0) {}
    any_unary(c2) : var(std::vector<double>(nloops, 1.0)) {}
    double get(int i) const noexcept {
        if (var.type() == typeid(double))
            return boost::any_cast<const double&>(var);
        return boost::any_cast<const std::vector<double>&>(var)[i];
    }
};

static void any_method_1(bm::State& state) {
    c1 c;
    while (state.KeepRunning()) {
        any_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

static void any_method_2(bm::State& state) {
    c2 c;
    while (state.KeepRunning()) {
        any_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

struct nodefault {
    std::unique_ptr<double[]> p;
    nodefault(int n) : p(new double[n]) {
        std::fill(p.get(), p.get()+n, 1.0);
    }
    double operator[](int i) const noexcept { return p[i]; }
};

struct simple_unary {
    char type;
    union U {
        double val;
        nodefault arr;
        U(c1) : val(1.0) {}
        U(c2) : arr(100) {}
        ~U() {}
    } u;
    ~simple_unary() {
        if (type == 2)
            u.arr.~nodefault();
    }
    simple_unary(c1 c) : type(1), u(c) {}
    simple_unary(c2 c) : type(2), u(c) {}
    double get(int i) const noexcept {
        if (type == 1) return u.val;
        return u.arr[i];
    }
};

static void simple_method_1(bm::State& state) {
    c1 c;
    while (state.KeepRunning()) {
        simple_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

static void simple_method_2(bm::State& state) {
    c2 c;
    while (state.KeepRunning()) {
        simple_unary u(c);
        for (int i = 0; i < nloops; ++i)
            bm::DoNotOptimize(u.get(i));
    }
}

BENCHMARK(virtual_method_1);
BENCHMARK(virtual_method_2);
BENCHMARK(variant_method_1);
BENCHMARK(variant_method_2);
BENCHMARK(any_method_1);
BENCHMARK(any_method_2);
BENCHMARK(simple_method_1);
BENCHMARK(simple_method_2);

BENCHMARK_MAIN();
