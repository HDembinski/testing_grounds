#include "benchmark/benchmark.h"
namespace bm = benchmark;

template <typename Tag>
struct dispatch1;

template <>
struct dispatch1<int> {
    static int doit(int x) { return x+1; }
};

template <>
struct dispatch1<long> {
    static int doit(int x) { return x+2; }
};

struct int_tag {};
struct long_tag {};

int dispatch2(int x, int_tag) { return x+1; }
int dispatch2(int x, long_tag) { return x+2; }

static void run_dispatch1(bm::State& state) {
    while (state.KeepRunning()) {
        bm::DoNotOptimize(dispatch1<int>::doit(3) + dispatch1<long>::doit(3));
    }
}

static void run_dispatch2(bm::State& state) {
    while (state.KeepRunning()) {
        bm::DoNotOptimize(dispatch2(3, int_tag{}) + dispatch2(3, long_tag{}));
    }
}

BENCHMARK(run_dispatch1);
BENCHMARK(run_dispatch2);

BENCHMARK_MAIN();
