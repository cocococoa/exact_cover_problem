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

  auto option_handler = OptionHandler();
  for (auto pidx = 0; pidx < (int)prime_list.size(); ++pidx) {
    const auto prime = prime_list[pidx];
    // 横置き
    for (auto i = 0; i < size; ++i) {
      auto option = Option();
      const auto pos_item = option_handler.AddItem(i);
      option.AddPrimaryItem(pos_item);
      const auto num_item = option_handler.AddSecondaryItem(prime);
      option.AddSecondaryItem(num_item, 0);
      const auto ps = split(prime);
      for (auto j = 0; j < size; ++j) {
        auto cell_item = option_handler.AddSecondaryItem(i, j);
        option.AddSecondaryItem(cell_item, ps[j] + 1);
      }

      option_handler.AddOption(option);
    }
    // 縦置き
    for (auto j = 0; j < size; ++j) {
      auto option = Option();
      const auto pos_item = option_handler.AddItem(j);
      option.AddPrimaryItem(pos_item);
      const auto num_item = option_handler.AddSecondaryItem(prime);
      option.AddSecondaryItem(num_item, 0);
      const auto ps = split(prime);
      for (auto i = 0; i < size; ++i) {
        auto cell_item = option_handler.AddSecondaryItem(i, j);
        option.AddSecondaryItem(cell_item, ps[i] + 1);
      }
    }
  }

  const auto [num_primary_items, num_secondary_items, option_list,
              compile_to_raw] = option_handler.XCCCompile();
  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  runXCCSolver("prime square " + std::to_string(size), solver, save_solution);

  const auto print_solution = [size, &option_handler,
                               compile_to_raw = &compile_to_raw](
                                  const std::vector<int>& solution) {
    auto square = std::vector<int>(size * size, 0);
    for (const auto oidx : solution) {
      const auto& option = option_handler.GetOption(compile_to_raw->at(oidx));
      for (auto index = 1; index <= size; ++index) {
        const auto [item_ptr, color] = option.GetSecondaryItem(index);
        const auto i = item_ptr->Get<int>(0);
        const auto j = item_ptr->Get<int>(1);
        square[size * i + j] = color - 1;
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
  std::cout << "-----------------------------------------------------------\n"
            << "# Prime Square\n"
            << std::endl;
  findPrimeSquare(2, true);
  findPrimeSquare(3, true);
  findPrimeSquare(4, false);
  return 0;
}