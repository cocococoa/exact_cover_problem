#include <iostream>
#include <string>
#include <vector>

#include "common/common.h"

void nQueen(int size) {
  auto option_handler = OptionHandler();

  for (auto x = 0; x < size; ++x) {
    for (auto y = 0; y < size; ++y) {
      // (x, y) にクイーンを置いたら
      // どの 縦、横、右斜め、左斜め を占有するか？
      const auto i1 = option_handler.AddItem('r', x);
      const auto i2 = option_handler.AddItem('c', y);
      const auto i3 = option_handler.AddSecondaryItem('a', x + y);
      const auto i4 = option_handler.AddSecondaryItem('b', x - y);
      auto option = Option();
      option.AddPrimaryItems(i1, i2);
      option.AddSecondaryItem(i3, 0);
      option.AddSecondaryItem(i4, 0);
      option_handler.AddOption(option);
    }
  }

  const auto [num_primary_items, num_secondary_items, option_list, _] =
      option_handler.XCCCompile();
  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  runXCCSolver("nqueen " + std::to_string(size), solver, false);
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
