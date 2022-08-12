#include "dancing_links.h"

void LangfordPair(int size, bool show_general_result = false,
                  bool show_specific_result = false) {
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
  solver.Solve(not(show_general_result or show_specific_result));
  std::cout << "Done" << std::endl;

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;
  std::cout << "Num solutions without symmetry: " << num_solutions / 2
            << std::endl;
  if (show_general_result) {
    // General
    for (auto i = 0; i < num_solutions; ++i)
      std::cout << solver.GetPrettySolution(i) << std::endl;
  }
  if (show_specific_result) {
    // Specific
    auto array = std::vector<int>(2 * size, 0);
    for (auto i = 0; i < num_solutions; ++i) {
      const auto sol = solver.GetSolution(i);
      for (const auto oidx : sol) {
        const auto& option = option_list[oidx];
        const auto l = option[0];
        const auto r = option[1];
        const auto number = option[2] - 2 * size + 1;
        array[l] = number;
        array[r] = number;
      }
      std::cout << Tostr(array.begin(), array.end(), "") << std::endl;
    }
  }
}

int main() {
  LangfordPair(3, true, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(4, true, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(7, false, true);
  std::cout << "======================================" << std::endl;
  LangfordPair(8, false, true);
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
