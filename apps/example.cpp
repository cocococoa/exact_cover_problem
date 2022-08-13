#include "solver/dancing_links.h"

void tryTAOCPExample() {
  const int num_items = 7;
  const auto option_list = std::vector<std::vector<int>>{
      {2, 4}, {0, 3, 6}, {1, 2, 5}, {0, 3, 5}, {1, 6}, {3, 4, 6}};

  auto solver = ExactCoverProblemSolver(num_items, option_list);
  solver.PrintCurrentLink();

  std::cout << "Find exact cover via daincing links" << std::endl;
  solver.Solve();
  std::cout << "Done" << std::endl;

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;
  for (auto i = 0; i < num_solutions; ++i)
    std::cout << solver.GetPrettySolution(i) << std::endl;
}

int main() {
  tryTAOCPExample();
  return 0;
}
