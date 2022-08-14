#include <iostream>
#include <vector>

#include "common.h"

void tryTAOCPExample() {
  const int num_items = 7;
  const auto option_list = std::vector<std::vector<int>>{
      {2, 4}, {0, 3, 6}, {1, 2, 5}, {0, 3, 5}, {1, 6}, {3, 4, 6}};

  auto solver = ExactCoverProblemSolver(num_items, option_list);
  solver.PrintCurrentLink();

  const auto num_solutions = runSolver(solver, true);
  for (auto i = 0; i < num_solutions; ++i)
    std::cout << solver.GetPrettySolution(i) << std::endl;
}

int main() {
  tryTAOCPExample();
  return 0;
}
