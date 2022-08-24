#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "common/common.h"

struct Board {
  int xdim;
  int ydim;
  std::vector<std::vector<int>> board;

  Board(int i_xdim, int i_ydim)
      : xdim(i_xdim), ydim(i_ydim), board(xdim, std::vector<int>(ydim, 0)) {}
  void Set(int x, int y, int v) { board[x][y] = v; }
  int Get(int x, int y) const { return board[x][y]; }
  char GetOr(int x, int y, char c) const {
    if (x < 0 or xdim <= x or y < 0 or ydim <= y) return c;
    return Get(x, y);
  }
  void Print() const {
    auto map = std::map<std::pair<std::pair<int, int>, std::pair<int, int>>,
                        std::string>();
    // 左・右・上・下
    map[{{0, 0}, {0, 0}}] = " ";  // 0
    map[{{1, 0}, {0, 0}}] = "?";  // 1
    map[{{0, 1}, {0, 0}}] = "?";  // 1
    map[{{0, 0}, {1, 0}}] = "?";  // 1
    map[{{0, 0}, {0, 1}}] = "?";  // 1
    map[{{0, 0}, {1, 1}}] = "│";  // 2
    map[{{1, 1}, {0, 0}}] = "─";  // 2
    map[{{0, 1}, {1, 0}}] = "└";  // 2
    map[{{0, 1}, {0, 1}}] = "┌";  // 2
    map[{{1, 0}, {0, 1}}] = "┐";  // 2
    map[{{1, 0}, {1, 0}}] = "┘";  // 2
    map[{{0, 1}, {1, 1}}] = "├";  // 3
    map[{{1, 1}, {0, 1}}] = "┬";  // 3
    map[{{1, 0}, {1, 1}}] = "┤";  // 3
    map[{{1, 1}, {1, 0}}] = "┴";  // 3
    map[{{1, 1}, {1, 1}}] = "┼";  // 4
    const auto get_key = [](int l, int r, int u, int d) {
      return std::make_pair(std::make_pair(l, r), std::make_pair(u, d));
    };
    for (auto y = ydim; y >= -1; --y) {
      for (auto x = -1; x <= xdim; ++x) {
        const auto ld = GetOr(x, y, -1);
        const auto lu = GetOr(x, y + 1, -1);
        const auto rd = GetOr(x + 1, y, -1);
        const auto ru = GetOr(x + 1, y + 1, -1);
        const auto key = get_key(ld != lu, rd != ru, lu != ru, ld != rd);
        std::cout << map.at(key);
      }
      std::cout << std::endl;
    }
  }
};

Board loadShikaku(const std::string& path, int xdim, int ydim) {
  auto ifs = std::ifstream{path};
  if (not ifs.good())
    throw std::runtime_error("Input file does not exist: " + path);
  auto ret = Board(xdim, ydim);
  auto y = ydim - 1;
  while (not ifs.eof()) {
    auto s = std::string();
    std::getline(ifs, s);
    auto ss = std::stringstream();
    ss << s;
    for (auto x = 0; x < xdim; ++x) {
      auto v = 0;
      ss >> v;
      ret.Set(x, y, v);
    }
    --y;
  }
  return ret;
}

void shikaku(const std::string& path, int xdim, int ydim, int index) {
  const auto shikaku = loadShikaku(path, xdim, ydim);

  auto option_handler = OptionHandler();

  // TODO(masaki.ono): fix (4重ループ)
  for (auto xl = 0; xl < xdim; ++xl) {
    for (auto xu = xl; xu < xdim; ++xu)
      for (auto yl = 0; yl < ydim; ++yl) {
        for (auto yu = yl; yu < ydim; ++yu) {
          // 長方形 (xl, xu, yl, yu) に数字がただ一つ含まれるか？
          int num = 0;
          int num_x = 0;
          int num_y = 0;
          for (auto x = xl; x <= xu; ++x) {
            for (auto y = yl; y <= yu; ++y) {
              const auto tmp = shikaku.Get(x, y);
              if (tmp != 0) {
                if (num > 0)  // 2つめの数字
                  num = -1;
                else if (num == 0) {
                  num = tmp;
                  num_x = x;
                  num_y = y;
                }
              }
            }
          }

          if (num <= 0) continue;
          if (num != (xu - xl + 1) * (yu - yl + 1)) continue;

          auto option = Option();
          const auto num_item =
              option_handler.AddItem("num", num_x, num_y, num);
          option.AddPrimaryItem(num_item);
          for (auto x = xl; x <= xu; ++x) {
            for (auto y = yl; y <= yu; ++y) {
              const auto cell_item = option_handler.AddItem("cell", x, y);
              option.AddPrimaryItem(cell_item);
            }
          }
          option_handler.AddOption(option);
        }
      }
  }

  const auto [num_items, option_list, compile_to_raw] =
      option_handler.XCCompile();
  auto solver = ExactCoverProblemSolver(num_items, option_list);
  runXCSolver("shikaku " + std::to_string(index), solver, true);

  const auto option_idx_list = solver.GetSolution(0);
  auto solved_shikaku = shikaku;
  for (const auto oidx : option_idx_list) {
    const auto& option = option_handler.GetOption(compile_to_raw.at(oidx));
    const auto num_x = option.GetPrimaryItem(0)->Get<int>(1);
    const auto num_y = option.GetPrimaryItem(0)->Get<int>(2);
    const auto num = option.GetPrimaryItem(0)->Get<int>(3);
    const auto v =
        num_x * 10000 + num_y * 100 + num;  // TODO(masaki.ono): fix index
    for (int i = 1; i <= num; ++i) {
      const auto x = option.GetPrimaryItem(i)->Get<int>(1);
      const auto y = option.GetPrimaryItem(i)->Get<int>(2);
      solved_shikaku.Set(x, y, v);
    }
  }
  std::cout << "Solution: \n" << std::endl;
  solved_shikaku.Print();
  std::cout << std::endl;
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# Shikaku\n"
            << std::endl;
  std::cout << "-------------------------------" << std::endl;
  shikaku("data/shikaku/0.txt", 7, 7, 0);
  std::cout << "-------------------------------" << std::endl;
  shikaku("data/shikaku/1.txt", 10, 10, 1);
  std::cout << "-------------------------------" << std::endl;
  shikaku("data/shikaku/2.txt", 20, 20, 2);
}
