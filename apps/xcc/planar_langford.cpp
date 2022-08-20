#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "common/common.h"

void langfordPair(int size, bool show_result = false) {
  auto option_handler = OptionHandler();

  for (auto i = 1; i <= size; ++i) {
    for (auto j = 0; j < 2 * size; ++j) {
      const auto l = j;
      const auto r = j + i + 1;
      if (r >= 2 * size) continue;

      const auto i1 = option_handler.AddItem('x', l);
      const auto i2 = option_handler.AddItem('x', r);
      const auto i3 = option_handler.AddItem('i', i);
      auto option = Option();
      option.AddPrimaryItems(i1, i2, i3);
      const auto half = (i + 1) / 2;

      // Upper option
      {
        auto uop = option;
        for (auto y = 0; y < half; ++y) {
          const auto item = option_handler.AddSecondaryItem('u', l, y);
          uop.AddSecondaryItem(item, 0);
        }
        for (auto x = l + 1; x <= r - 1; ++x) {
          const auto item = option_handler.AddSecondaryItem('u', x, half - 1);
          uop.AddSecondaryItem(item, 0);
        }
        for (auto y = 0; y < half; ++y) {
          const auto item = option_handler.AddSecondaryItem('u', r, y);
          uop.AddSecondaryItem(item, 0);
        }
        option_handler.AddOption(uop);
      }

      if (i == size) continue;

      // Downer option
      {
        auto dop = option;
        for (auto y = 0; y < half; ++y) {
          const auto item = option_handler.AddSecondaryItem('d', l, y);
          dop.AddSecondaryItem(item, 0);
        }
        for (auto x = l + 1; x <= r - 1; ++x) {
          const auto item = option_handler.AddSecondaryItem('d', x, half - 1);
          dop.AddSecondaryItem(item, 0);
        }
        for (auto y = 0; y < half; ++y) {
          const auto item = option_handler.AddSecondaryItem('d', r, y);
          dop.AddSecondaryItem(item, 0);
        }
        option_handler.AddOption(dop);
      }
    }
  }

  const auto [num_primary_items, num_secondary_items, option_list,
              compile_to_raw] = option_handler.XCCCompile();

  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  const auto num_solutions =
      runXCCSolver("planar langford " + std::to_string(size), solver, true);

  auto solution_pool = std::set<std::string>();
  for (auto i = 0; i < num_solutions; ++i) {
    auto array = std::vector<int>(2 * size, 0);
    const auto sol = solver.GetSolution(i);
    for (const auto oidx : sol) {
      const auto& option = option_handler.GetOption(compile_to_raw.at(oidx));
      const auto l = option.GetPrimaryItem(0)->Get<int>(1);
      const auto r = option.GetPrimaryItem(1)->Get<int>(1);
      const auto number = option.GetPrimaryItem(2)->Get<int>(1);
      array[l] = number;
      array[r] = number;
    }
    const auto seq = tostr(array.begin(), array.end(), " ");
    const auto rev = tostr(array.rbegin(), array.rend(), " ");
    if (seq <= rev)
      solution_pool.insert(seq);
    else
      solution_pool.insert(rev);
  }
  std::cout << "Num planar langford pairs: " << solution_pool.size()
            << std::endl;
  if (show_result) {
    for (const auto& sol : solution_pool) std::cout << sol << std::endl;
  }
}

int main() {
  // Reference:
  // https://oeis.org/A125762

  std::cout << "-----------------------------------------------------------\n"
            << "# Planar langford pair\n"
            << std::endl;
  std::cout << "-------------------------------" << std::endl;
  langfordPair(3, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(4, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(7, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(8, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(11, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(12, true);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(15);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(16);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(19);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(20);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(23);
  std::cout << "-------------------------------" << std::endl;
  langfordPair(24);
  return 0;
}
