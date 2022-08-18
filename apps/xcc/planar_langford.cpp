#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "common/common.h"

void langfordPair(int size, bool show_result = false) {
  const auto half_size = (size + 1) / 2;
  const auto num_primary_items = 2 * size + size;
  const auto num_secondary_items = 2 * 2 * size * half_size;
  const auto offset0 = 0;
  const auto offset1 = 2 * size;
  const auto offset2 = 2 * size + size;
  const auto offset3 = 2 * size + size + 2 * size * half_size;

  auto option_list = std::vector<std::vector<std::pair<int, int>>>{};
  for (auto i = 1; i <= size; ++i) {
    for (auto j = 0; j < 2 * size; ++j) {
      const auto l = j;
      const auto r = j + i + 1;
      if (r >= 2 * size) continue;

      const auto option = std::vector<std::pair<int, int>>{
          {offset0 + l, 0}, {offset0 + r, 0}, {offset1 + i - 1, 0}};
      const auto half = (i + 1) / 2;

      // Upper option
      {
        auto uop = option;
        for (auto y = 0; y < half; ++y) {
          uop.push_back({offset2 + half_size * l + y, 0});
        }
        for (auto x = l + 1; x <= r - 1; ++x) {
          uop.push_back({offset2 + half_size * x + half - 1, 0});
        }
        for (auto y = 0; y < half; ++y) {
          uop.push_back({offset2 + half_size * r + y, 0});
        }
        std::sort(uop.begin(), uop.end());
        option_list.push_back(uop);
      }

      if (i == size) continue;

      // Downer option
      {
        auto dop = option;
        for (auto y = 0; y < half; ++y) {
          dop.push_back({offset3 + half_size * l + y, 0});
        }
        for (auto x = l + 1; x <= r - 1; ++x) {
          dop.push_back({offset3 + half_size * x + half - 1, 0});
        }
        for (auto y = 0; y < half; ++y) {
          dop.push_back({offset3 + half_size * r + y, 0});
        }
        std::sort(dop.begin(), dop.end());
        option_list.push_back(dop);
      }
    }
  }

  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  const auto num_solutions =
      runXCCSolver("planar langford " + std::to_string(size), solver, true);

  auto solution_pool = std::set<std::string>();
  for (auto i = 0; i < num_solutions; ++i) {
    auto array = std::vector<int>(2 * size, 0);
    const auto sol = solver.GetSolution(i);
    for (const auto oidx : sol) {
      const auto& option = option_list[oidx];
      const auto l = option[0].first;
      const auto r = option[1].first;
      const auto number = option[2].first - 2 * size + 1;
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
