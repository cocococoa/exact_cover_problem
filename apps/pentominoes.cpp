#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "solver/dancing_links.h"

struct Board {
  int xlen;
  int ylen;
  std::vector<std::vector<char>> board;
  Board(int i_xlen, int i_ylen) : xlen(i_xlen), ylen(i_ylen) {
    board.resize(xlen);
    for (auto&& b : board) b.resize(ylen, ' ');
  }

  void Set(int x, int y, char c) { board[x][y] = c; }
  char Get(int x, int y) const { return board[x][y]; }
  char GetOr(int x, int y, char c) const {
    if (x < 0 or xlen <= x or y < 0 or ylen <= y) return c;
    return Get(x, y);
  }
  void Print() const {
    for (auto y = ylen - 1; y >= 0; --y) {
      for (auto x = 0; x < xlen; ++x) {
        std::cout << Get(x, y);
      }
      std::cout << std::endl;
    }
  }
  void PrettyPrint() const {
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
    for (auto y = ylen; y >= -1; --y) {
      for (auto x = -1; x <= xlen; ++x) {
        const auto ld = GetOr(x, y, '?');
        const auto lu = GetOr(x, y + 1, '?');
        const auto rd = GetOr(x + 1, y, '?');
        const auto ru = GetOr(x + 1, y + 1, '?');
        const auto key = get_key(ld != lu, rd != ru, lu != ru, ld != rd);
        std::cout << map.at(key);
      }
      std::cout << std::endl;
    }
  }
};
int countDiff(const Board& lhs, const Board& rhs) {
  auto diff = 0;
  for (auto x = 0; x < lhs.xlen; ++x)
    for (auto y = 0; y < lhs.ylen; ++y)
      if (lhs.Get(x, y) != rhs.Get(x, y)) diff++;
  return diff;
}
bool operator==(const Board& lhs, const Board& rhs) {
  if (lhs.xlen != rhs.xlen) return false;
  if (lhs.ylen != rhs.ylen) return false;
  const auto diff = countDiff(lhs, rhs);
  return diff == 0;
}

struct Pentomino {
  int symmetry;
  std::vector<std::pair<int, int>> shape;

  void Translate(const int dx, const int dy) {
    for (auto i = 0; i < (int)shape.size(); ++i) {
      const auto [x, y] = shape[i];
      shape[i] = {x + dx, y + dy};
    }
  }
  Pentomino Translated(const int dx, const int dy) const {
    auto ret = *this;
    ret.Translate(dx, dy);
    return ret;
  }
  void Rotate() {
    for (auto i = 0; i < (int)shape.size(); ++i) {
      const auto [x, y] = shape[i];
      shape[i] = {-y, x};
    }
  }
  void Rotate(int count) {
    for (auto i = 0; i < count; ++i) Rotate();
  }
  void Reflect() {
    for (auto i = 0; i < (int)shape.size(); ++i) {
      const auto [x, y] = shape[i];
      shape[i] = {-x, y};
    }
  }
  bool InBox(const int min_x, const int max_x, const int min_y,
             const int max_y) const {
    auto ret = true;
    for (const auto& [x, y] : shape) {
      if (x < min_x) ret = false;
      if (max_x < x) ret = false;
      if (y < min_y) ret = false;
      if (max_y < y) ret = false;
    }
    return ret;
  }
};
std::map<int, char> construct() {
  auto ret = std::map<int, char>();
  ret[0] = 'X';
  ret[1] = ' ';
  ret[2] = 'I';
  ret[3] = 'Z';
  ret[4] = 'U';
  ret[5] = 'P';
  ret[6] = 'T';
  ret[7] = 'Y';
  ret[8] = 'V';
  ret[9] = 'F';
  ret[10] = 'L';
  ret[11] = 'N';
  ret[12] = 'W';
  return ret;
}

void pentminoes() {
  const auto num_pentominoes = 13;
  const auto do_reflection = true;

  const auto map = construct();
  const auto x00 = Pentomino{1, {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, -1}}};
  const auto x01 = Pentomino{1, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}};
  const auto x02 = Pentomino{2, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}};
  const auto x03 = Pentomino{2, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, -1}}};
  const auto x04 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, 1}}};
  const auto x05 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}}};
  const auto x06 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, 2}}};
  const auto x07 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {2, 1}}};
  const auto x08  // 解そのものに回転対称性があるため絞る目的でsym=1とする
      = Pentomino{1, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
  const auto x09 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {1, -1}, {2, 1}}};
  const auto x10 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}}};
  const auto x11 = Pentomino{4, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 1}}};
  const auto x12 = Pentomino{4, {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}}};
  const auto pent_list = std::vector<Pentomino>{
      x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x10, x11, x12};

  // board は 8 x 8
  const auto num_items = num_pentominoes + 8 * 8;
  const auto coord2index = [](int x, int y) {
    return num_pentominoes + x * 8 + y;
  };
  auto option_list = std::vector<std::vector<int>>();
  auto shape_list = std::vector<Pentomino>();
  for (auto pidx = 0; pidx < num_pentominoes; ++pidx) {
    for (auto ref = 0; ref < 2; ++ref) {
      for (auto sym = 0; sym < pent_list[pidx].symmetry; ++sym) {
        auto pentomino = pent_list[pidx];
        pentomino.Rotate(sym + 1);
        if (ref == 1) pentomino.Reflect();

        // 平行移動開始
        for (auto dx = -20; dx <= 20; ++dx) {
          for (auto dy = -20; dy <= 20; ++dy) {
            // 正方形は穴のため
            if (pidx == 1)
              if (not(dx == 4 and dy == 3)) continue;

            const auto tmp = pentomino.Translated(dx, dy);
            if (tmp.InBox(0, 7, 0, 7)) {
              auto option = std::vector<int>{pidx};
              for (const auto& [x, y] : tmp.shape)
                option.emplace_back(coord2index(x, y));
              std::sort(option.begin(), option.end());
              option_list.push_back(option);
              shape_list.push_back(tmp);
            }
          }
        }
        // 平行移動終了
      }

      if (do_reflection and
          (
              //  pidx == 3 or
              // 解には線対称性があるため、絞る目的でx03は反転しない
              pidx == 5 or pidx == 7 or pidx == 9 or pidx == 10 or pidx == 11))
        ;
      else
        break;
    }
  }

  const auto start = std::chrono::high_resolution_clock::now();
  auto solver = ExactCoverProblemSolver(num_items, option_list);
  std::cout << "Find exact cover via dancing links" << std::endl;
  solver.SolveMultiThread(true);
  std::cout << "Done" << std::endl;
  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;
  const auto end = std::chrono::high_resolution_clock::now();
  const auto elapsed_sec =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "Elapsed: " << elapsed_sec.count() << " [seconds]" << std::endl;

  // Print solution
  const auto print = [&](int sol) {
    const auto option_idx_list = solver.GetSolution(sol);
    auto board = Board(8, 8);
    for (auto i = 0; i < num_pentominoes; ++i) {
      const auto oidx = option_idx_list[i];
      const auto shape = shape_list[oidx];
      for (const auto& [x, y] : shape.shape) {
        board.Set(x, y, map.at(i));
      }
    }
    std::cout << "================================" << std::endl;
    std::cout << "Solution: " << sol << std::endl;
    // board.Print();
    board.PrettyPrint();
  };
  print(0);
  print(10);
}
int main() {
  pentminoes();
  return 0;
}