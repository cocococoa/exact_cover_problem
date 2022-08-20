#include <iostream>
#include <string>
#include <vector>

#include "common/common.h"

void langfordPair(int size, bool show_result = false) {
  auto option_handler = OptionHandler();
  for (auto i = 1; i <= size; ++i) {
    for (auto j = 0; j < 2 * size; ++j) {
      const auto l = j;
      const auto r = j + i + 1;
      if (r < 2 * size) {
        const auto i1 = option_handler.AddItem('x', l);
        const auto i2 = option_handler.AddItem('x', r);
        const auto i3 = option_handler.AddItem('i', i);
        auto option = Option();
        option.AddPrimaryItems(i1, i2, i3);
        option_handler.AddOption(option);
      }
    }
  }

  const auto [num_items, option_list, compile_to_raw] =
      option_handler.Compile();
  auto solver = ExactCoverProblemSolver(num_items, option_list);
  const auto num_solutions =
      runXCSolver("langford " + std::to_string(size), solver, show_result);

  if (show_result) {
    for (auto i = 0; i < num_solutions; ++i) {
      const auto sol = solver.GetSolution(i);
      for (const auto oidx : sol) {
        const auto& option = option_handler.GetOption(compile_to_raw.at(oidx));
        std::cout << option.Str() << std::endl;
      }
    }
  }
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# Langford pair\n"
            << std::endl;
  std::cout << "-------------------------------" << std::endl;
  langfordPair(3, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(4, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(7);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(8);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(11);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(12);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(15);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(16);
  return 0;
}
