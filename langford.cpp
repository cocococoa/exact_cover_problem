#include "dancing_links.h"

void LangfordPair(int size, bool show_result = false) {
  const int num_items = 2 * size + size;
  auto option_list = std::vector<std::vector<int>>{};
  for (auto i = 1; i <= size; ++i) {
    for (auto j = 0; j < 2 * size; ++j) {
      const auto l = j;
      const auto r = j + i + 1;
      if (r < 2 * size) {
        option_list.emplace_back(std::vector<int>{l, r, 2 * size + i - 1});
      }
    }
  }
  auto solver = ExactCoverProblemSolver(num_items, option_list);

  std::cout << "Find exact cover via daincing links" << std::endl;
  solver.Solve(not show_result);

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions / 2 << std::endl;
  if (show_result) {
    for (auto i = 0; i < num_solutions; ++i)
      std::cout << solver.GetPrettySolution(i) << std::endl;
  }
}

int main() {
  LangfordPair(3, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(4, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(7, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(8);
  std::cout << "======================================" << std::endl;
  LangfordPair(11);
  std::cout << "======================================" << std::endl;
  LangfordPair(12);
  // 約3分かかる
  // std::cout << "======================================" << std::endl;
  // LangfordPair(15);
  // 約30分かかる
  // std::cout << "======================================" << std::endl;
  // LangfordPair(16);
  return 0;
}
