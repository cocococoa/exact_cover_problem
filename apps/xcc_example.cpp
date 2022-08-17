#include <iostream>
#include <vector>

#include "common.h"

void tryXCCExample() {
  const auto num_primary_items = 3;
  const auto num_secondary_items = 2;
  const auto option_list = std::vector<std::vector<std::pair<int, int>>>{
      {{0, 0}, {1, 0}, {3, 0}, {4, 1}},  // p q x y:A
      {{0, 0}, {2, 0}, {3, 1}, {4, 0}},  // p q x:A y
      {{0, 0}, {3, 2}},                  // p x:B
      {{1, 0}, {3, 1}},                  // q x:A
      {{2, 0}, {4, 2}}                   // r y:B
  };

  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  solver.PrintCurrentLink();

  const auto num_solutions = runXCCSolver("XCC example", solver, true);
  for (auto i = 0; i < num_solutions; ++i)
    std::cout << solver.GetPrettySolution(i) << std::endl;
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# XCC Example\n"
            << std::endl;
  tryXCCExample();
  return 0;
}
