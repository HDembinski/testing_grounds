#include <iostream>
#include <benchmark/benchmark.h>
namespace bm = benchmark;

static void normal_loop(bm::State& state) {
  const auto n = state.range(0);
  auto sum = 0u;
  while (state.KeepRunning()) {
    for (unsigned i = 0; i < n; ++i)
      bm::DoNotOptimize(sum += i);
  }
}

struct range {
    range(int n) : n_(n) {}
    struct iterator {
        int value;
        int operator*() const noexcept { return value; }
        inline bool operator!=(const iterator& rhs) const noexcept {
            return value != rhs.value;
        }
        inline void operator++() noexcept { ++value; }
    };
    int n_;
    iterator begin() noexcept { return {0}; }
    iterator end() noexcept { return {n_}; }
};

static void range_loop(bm::State& state) {
  const auto n = state.range(0);
  auto sum = 0u;
  while (state.KeepRunning()) {
    for (auto i : range(n))
      bm::DoNotOptimize(sum += i);
  }
}

BENCHMARK(normal_loop)->Arg(2048);
BENCHMARK(range_loop)->Arg(2048);

BENCHMARK_MAIN();
