#include <iostream>
#include <string>
#include <vector>

#include "common.h"

void nQueen(int size) {
  const auto num_items = size + size + (2 * size - 1) + (2 * size - 1);
  const auto offset0 = 0;
  const auto offset1 = size;
  const auto offset2 = size + size;
  const auto offset3 = size + size + (2 * size - 1);
  auto option_list = std::vector<std::vector<int>>();

  for (auto x = 0; x < size; ++x) {
    for (auto y = 0; y < size; ++y) {
      // (x, y) にクイーンを置いたら
      // どの 縦、横、右斜め、左斜め を占有するか？
      auto option = std::vector<int>();
      option.push_back(offset0 + x);
      option.push_back(offset1 + y);
      option.push_back(offset2 + x + y);
      option.push_back(offset3 + x - y + size - 1);
      option_list.push_back(option);
    }
  }
  // Secondary item 用のスラックオプション
  for (auto i = offset2; i < num_items; ++i) {
    option_list.push_back({i});
  }

  auto solver = ExactCoverProblemSolver(num_items, option_list);
  runXCSolver("nqueen " + std::to_string(size), solver, false);
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# N Queen\n"
            << std::endl;
  // N   : Solutions
  // 1   : 1
  // 2   : 0
  // 3   : 0
  // 4   : 2
  // 5   : 10
  // 6   : 4
  // 7   : 40
  // 8   : 92
  // 9   : 352
  // 10  : 724
  // 11  : 2680
  // 12  : 14,200
  // 13  : 73,712
  // 14  : 365,596
  // 15  : 2,279,184
  // 16  : 14,772,512
  // 17  : 95,815,104
  // 18  : 666,090,624
  for (auto size = 1; size <= 16; ++size) {
    std::cout << "-------------------------------" << std::endl;
    std::cout << "Board size: " << size << std::endl;
    nQueen(size);
  }
  return 0;
}
