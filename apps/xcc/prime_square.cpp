#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "common/common.h"

int pow10(int e) {
  auto ret = 1;
  while (e > 0) {
    ret *= 10;
    e--;
  }
  return ret;
}

std::vector<int> split(int x) {
  auto ret = std::vector<int>();
  while (x > 0) {
    ret.push_back(x % 10);
    x /= 10;
  }
  std::reverse(ret.begin(), ret.end());
  return ret;
}

std::vector<int> findPrimes(int min, int max) {
  auto sieve = std::vector<int>(max + 1, 1);
  sieve[0] = sieve[1] = 0;
  for (auto i = 2; i <= max; ++i) {
    if (sieve[i] == 0) continue;
    for (auto j = 2 * i; j <= max; j += i) sieve[j] = 0;
  }

  auto primes = std::vector<int>();
  for (auto i = min; i <= max; ++i) {
    if (sieve[i] == 1) primes.push_back(i);
  }
  return primes;
}

void findPrimeSquare(int size, bool save_solution) {
  const auto prime_list = findPrimes(pow10(size - 1), pow10(size) - 1);

  const auto num_primary_items = 2 * size;
  const auto num_secondary_items = size * size + (int)prime_list.size();
  const auto offset0 = 0;
  const auto offset1 = size;
  const auto offset2 = 2 * size;
  const auto offset3 = 2 * size + size * size;
  auto option_list = std::vector<std::vector<std::pair<int, int>>>();

  for (auto pidx = 0; pidx < (int)prime_list.size(); ++pidx) {
    const auto prime = prime_list[pidx];
    // 横置き
    for (auto i = 0; i < size; ++i) {
      auto option = std::vector<std::pair<int, int>>();
      option.push_back({offset0 + i, 0});
      const auto ps = split(prime);
      for (auto s = 0; s < size; ++s) {
        option.push_back({offset2 + size * i + s, ps[s] + 1});
      }
      option.push_back({offset3 + pidx, 0});
      option_list.push_back(option);
    }
    // 縦置き
    for (auto j = 0; j < size; ++j) {
      auto option = std::vector<std::pair<int, int>>();
      option.push_back({offset1 + j, 0});
      const auto ps = split(prime);
      for (auto s = 0; s < size; ++s) {
        option.push_back({offset2 + size * s + j, ps[s] + 1});
      }
      option.push_back({offset3 + pidx, 0});
      option_list.push_back(option);
    }
  }

  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  runXCCSolver("prime square " + std::to_string(size), solver, save_solution);

  const auto print_solution = [&](const auto& solution) {
    auto square = std::vector<int>(size * size, 0);
    for (const auto oidx : solution) {
      const auto& option = option_list[oidx];
      for (const auto [item, color] : option) {
        if (color > 0) square[item - offset2] = color - 1;
      }
    }
    for (auto i = 0; i < size; ++i) {
      for (auto j = 0; j < size; ++j) {
        std::cout << square[size * i + j];
      }
      std::cout << std::endl;
    }
  };
  if (save_solution) {
    std::cout << "Solution 0" << std::endl;
    print_solution(solver.GetSolution(0));
    std::cout << "Solution 10" << std::endl;
    print_solution(solver.GetSolution(10));
    std::cout << "Solution 20" << std::endl;
    print_solution(solver.GetSolution(20));
  }
}

int main() {
  findPrimeSquare(2, true);
  findPrimeSquare(3, true);
  findPrimeSquare(4, false);
  return 0;
}