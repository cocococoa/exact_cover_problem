#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "common/common.h"

struct Board {
  int xdim;
  int ydim;
  std::vector<std::vector<int>> board;

  static Board Zero(int i_xdim, int i_ydim) {
    auto board = Board();
    board.xdim = i_xdim;
    board.ydim = i_ydim;
    board.board.resize(i_xdim, std::vector<int>(i_ydim, 0));
    return board;
  }
  void Set(int x, int y, int v) { board[x][y] = v; }
  int Get(int x, int y) const { return board[x][y]; }
  int XDim() const { return xdim; }
  int YDim() const { return ydim; }
  std::string Str() const {
    auto ss = std::stringstream();
    for (auto y = 0; y < YDim(); ++y) {
      for (auto x = 0; x < XDim(); ++x) {
        if (Get(x, y) == 1)
          ss << " ";
        else
          ss << "■";
      }
      ss << std::endl;
    }
    return ss.str();
  }
};

void addOption(bool is_vertical, int target, int xdim, int ydim, int depth,
               int tmp_ub, OptionHandler& option_handler,
               const std::vector<int>& tmp_vec,
               const std::vector<int>& pattern) {
  // TODO(masaki.ono): 問題が大きいとこの関数がボトルネックになるため、改善する
  const auto vec2option = [&](const std::vector<int>& vec) {
    auto option = Option();
    if (is_vertical) {
      const auto vitem = option_handler.AddItem('x', target);
      option.AddPrimaryItem(vitem);
      for (auto y = 0; y < ydim; ++y) {
        const auto item = option_handler.AddSecondaryItem('x', target, 'y', y);
        const auto color = vec[y];
        option.AddSecondaryItem(item, color);
      }
    } else {
      const auto hitem = option_handler.AddItem('y', target);
      option.AddPrimaryItem(hitem);
      for (auto x = 0; x < xdim; ++x) {
        const auto item = option_handler.AddSecondaryItem('x', x, 'y', target);
        const auto color = vec[x];
        option.AddSecondaryItem(item, color);
      }
    }
    return option;
  };

  if (is_vertical) {
    for (auto y = 0; y < ydim; ++y) {
      const auto lb = tmp_ub + 1 + y;
      const auto ub = lb + pattern[depth];
      if (ub > ydim) continue;

      auto vec = tmp_vec;
      for (auto i = lb; i < ub; ++i) vec[i] = 2;
      if (depth + 1 == (int)pattern.size()) {
        option_handler.AddOption(vec2option(vec));
        continue;
      }
      addOption(is_vertical, target, xdim, ydim, depth + 1, ub, option_handler,
                vec, pattern);
    }
  } else {
    for (auto x = 0; x < xdim; ++x) {
      const auto lb = tmp_ub + 1 + x;
      const auto ub = lb + pattern[depth];
      if (ub > xdim) continue;

      auto vec = tmp_vec;
      for (auto i = lb; i < ub; ++i) vec[i] = 2;
      if (depth + 1 == (int)pattern.size()) {
        option_handler.AddOption(vec2option(vec));
        continue;
      }
      addOption(is_vertical, target, xdim, ydim, depth + 1, ub, option_handler,
                vec, pattern);
    }
  }
}

void solvePixelCrossImpl(const std::vector<std::vector<int>>& vpattern_list,
                         const std::vector<std::vector<int>>& hpattern_list) {
  const auto xdim = (int)vpattern_list.size();
  const auto ydim = (int)hpattern_list.size();

  auto option_handler = OptionHandler();

  for (auto x = 0; x < xdim; ++x) {
    const auto& vpattern = vpattern_list[x];
    auto vec = std::vector<int>(ydim, 1);
    addOption(true, x, xdim, ydim, 0, -1, option_handler, vec, vpattern);
  }
  for (auto y = 0; y < ydim; ++y) {
    const auto& hpattern = hpattern_list[y];
    auto vec = std::vector<int>(xdim, 1);
    addOption(false, y, xdim, ydim, 0, -1, option_handler, vec, hpattern);
  }

  const auto [num_primary_items, num_secondary_items, option_list,
              compile_to_raw] = option_handler.XCCCompile();
  auto solver = ExactCoverWithColorsSolver(num_primary_items,
                                           num_secondary_items, option_list);
  runXCCSolver(
      "pixel cross " + std::to_string(xdim) + ", " + std::to_string(ydim),
      solver, true);

  const auto print_solution = [xdim, ydim, &option_handler,
                               compile_to_raw = &compile_to_raw](
                                  const std::vector<int>& solution) {
    auto board = Board::Zero(xdim, ydim);
    for (const auto oidx : solution) {
      const auto& option = option_handler.GetOption(compile_to_raw->at(oidx));
      for (auto itr = option.SBegin(); itr != option.SEnd(); ++itr) {
        const auto [item_ptr, color] = *itr;
        const auto x = item_ptr->Get<int>(1);
        const auto y = item_ptr->Get<int>(3);
        board.Set(x, y, color);
      }
    }
    std::cout << board.Str() << std::endl;
  };
  for (auto i = 0; i < solver.NumSolutions(); ++i) {
    std::cout << "-------------------------------" << std::endl;
    print_solution(solver.GetSolution(i));
  }
}

void solvePixelCross() {
  {
    // https://ja.wikipedia.org/wiki/お絵かきロジック
    const auto vpattern_list = std::vector<std::vector<int>>{
        std::vector<int>{4},       std::vector<int>{6},
        std::vector<int>{2, 1, 2}, std::vector<int>{1, 1, 1},

        std::vector<int>{1, 1, 1}, std::vector<int>{2, 1, 2},
        std::vector<int>{3, 2},    std::vector<int>{2, 1}};
    const auto hpattern_list = std::vector<std::vector<int>>{
        std::vector<int>{4},    std::vector<int>{2, 2},
        std::vector<int>{2, 2}, std::vector<int>{8},

        std::vector<int>{2},    std::vector<int>{2, 2},
        std::vector<int>{2, 2}, std::vector<int>{4}};
    solvePixelCrossImpl(vpattern_list, hpattern_list);
  }
  {
    // https://www.minicgi.net/logic/logic.html?num=45639
    const auto vpattern_list = std::vector<std::vector<int>>{
        std::vector<int>{3, 3},       std::vector<int>{1, 1, 1, 3},
        std::vector<int>{2, 2, 1, 1}, std::vector<int>{2, 1},
        std::vector<int>{2, 3, 1},

        std::vector<int>{3, 5},       std::vector<int>{1, 2, 3, 3},
        std::vector<int>{2, 2, 7},    std::vector<int>{1, 10},
        std::vector<int>{1, 2, 9},

        std::vector<int>{1, 11},      std::vector<int>{2, 5, 3},
        std::vector<int>{3, 5},       std::vector<int>{3, 3},
        std::vector<int>{1, 1},
    };
    const auto hpattern_list = std::vector<std::vector<int>>{
        std::vector<int>{6},       std::vector<int>{2, 3, 3},
        std::vector<int>{4, 3, 3}, std::vector<int>{4, 2},
        std::vector<int>{3, 1},

        std::vector<int>{5},       std::vector<int>{3, 4},
        std::vector<int>{1, 1, 6}, std::vector<int>{2, 8},
        std::vector<int>{10},

        std::vector<int>{2, 4, 2}, std::vector<int>{10},
        std::vector<int>{3, 8},    std::vector<int>{2, 6},
        std::vector<int>{5},
    };
    solvePixelCrossImpl(vpattern_list, hpattern_list);
  }
  {
    // https://www.minicgi.net/logic/logic.html?num=45693
    const auto vpattern_list =
        std::vector<std::vector<int>>{std::vector<int>{20},
                                      std::vector<int>{4, 4},
                                      std::vector<int>{3, 3},
                                      std::vector<int>{2, 1, 1, 2},
                                      std::vector<int>{1, 12, 1},

                                      std::vector<int>{1, 2, 2, 1, 3, 1},
                                      std::vector<int>{1, 1, 2, 1, 1, 2, 1},
                                      std::vector<int>{1, 2, 1, 2, 1, 1},
                                      std::vector<int>{1, 3, 2, 2, 1, 1},
                                      std::vector<int>{1, 1, 2, 1, 1, 1},

                                      std::vector<int>{1, 4, 2, 1, 1, 1},
                                      std::vector<int>{1, 5, 2, 2, 1, 1},
                                      std::vector<int>{1, 2, 3, 2, 1, 1},
                                      std::vector<int>{1, 1, 4, 1, 1, 2, 1},
                                      std::vector<int>{1, 2, 6, 3, 1},

                                      std::vector<int>{1, 12, 1},
                                      std::vector<int>{2, 1, 1, 2},
                                      std::vector<int>{3, 3},
                                      std::vector<int>{4, 4},
                                      std::vector<int>{20}};
    const auto hpattern_list =
        std::vector<std::vector<int>>{std::vector<int>{20},
                                      std::vector<int>{4, 4},
                                      std::vector<int>{3, 3},
                                      std::vector<int>{2, 3, 3, 2},
                                      std::vector<int>{1, 2, 2, 2, 2, 1},

                                      std::vector<int>{1, 1, 1, 1, 1, 1},
                                      std::vector<int>{1, 1, 8, 1, 1},
                                      std::vector<int>{1, 3, 6, 1},
                                      std::vector<int>{1, 2, 6, 1},
                                      std::vector<int>{1, 1, 1, 3, 1},

                                      std::vector<int>{1, 1, 2, 2, 2, 1},
                                      std::vector<int>{1, 1, 2, 2, 2, 1},
                                      std::vector<int>{1, 4, 2, 4, 1},
                                      std::vector<int>{1, 1, 2, 1, 1},
                                      std::vector<int>{1, 4, 1, 1, 4, 1},

                                      std::vector<int>{1, 2, 4, 2, 1},
                                      std::vector<int>{2, 2, 2, 2},
                                      std::vector<int>{3, 8, 3},
                                      std::vector<int>{4, 4},
                                      std::vector<int>{20}};
    solvePixelCrossImpl(vpattern_list, hpattern_list);
  }
  {
    // https://www.puzzle-ch.com/logic/label.php?number=571
    const auto vpattern_list = std::vector<std::vector<int>>{
        std::vector<int>{4, 1},          std::vector<int>{3, 1, 1, 1},
        std::vector<int>{2, 2, 1},       std::vector<int>{3, 1, 1},
        std::vector<int>{2, 1, 1, 1, 1},

        std::vector<int>{1, 2, 2, 1},    std::vector<int>{4, 3, 1},
        std::vector<int>{2, 1, 1},       std::vector<int>{2, 3, 1},
        std::vector<int>{1, 2, 1, 1, 2},

        std::vector<int>{1, 1, 1, 1, 2}, std::vector<int>{2, 2, 2, 2},
        std::vector<int>{5, 1, 2},       std::vector<int>{1, 6, 1, 2},
        std::vector<int>{1, 3, 2, 2},

        std::vector<int>{1, 3, 1, 2},    std::vector<int>{1, 1, 5, 1},
        std::vector<int>{1, 6, 1},       std::vector<int>{1, 1, 3, 1},
        std::vector<int>{1, 3, 1}};
    const auto hpattern_list =
        std::vector<std::vector<int>>{std::vector<int>{3},
                                      std::vector<int>{2, 2},
                                      std::vector<int>{4, 2, 1},
                                      std::vector<int>{3, 1, 3},
                                      std::vector<int>{3, 9, 3},

                                      std::vector<int>{2, 5, 3, 1},
                                      std::vector<int>{2, 5, 2},
                                      std::vector<int>{1, 4, 3, 7},
                                      std::vector<int>{3, 2, 4},
                                      std::vector<int>{1, 4, 2, 2},

                                      std::vector<int>{1, 1, 1, 2, 4},
                                      std::vector<int>{3, 2, 2},
                                      std::vector<int>{2, 1},
                                      std::vector<int>{1, 7},
                                      std::vector<int>{2, 14}};
    solvePixelCrossImpl(vpattern_list, hpattern_list);
  }
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# Pixel Cross\n"
            << std::endl;
  solvePixelCross();
  return 0;
}