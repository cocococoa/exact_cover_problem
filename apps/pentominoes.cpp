#include <algorithm>
#include <iostream>
#include <vector>

#include "solver/dancing_links.h"

struct Board {
  const int xlen;
  const int ylen;
  std::vector<std::vector<char>> board;
  Board(int i_xlen, int i_ylen) : xlen(i_xlen), ylen(i_ylen) {
    board.resize(xlen);
    for (auto&& b : board) b.resize(ylen);
  }

  void Set(int x, int y, char c) { board[x][y] = c; }
  char Get(int x, int y) const { return board[x][y]; }
  void Print() const {
    for (auto y = ylen - 1; y >= 0; --y) {
      for (auto x = 0; x < xlen; ++x) {
        std::cout << Get(x, y);
      }
      std::cout << std::endl;
    }
  }
};

struct Pentominoes {
  const int symmetry;
  std::vector<std::pair<int, int>> shape;

  void Translate(const int dx, const int dy) {
    for (auto&& [x, y] : shape) {
      x += dx;
      y += dy;
    }
  }
  Pentominoes Translated(const int dx, const int dy) const {
    auto ret = *this;
    ret.Translate(dx, dy);
    return ret;
  }
  void Rotate() {
    for (auto&& [x, y] : shape) {
      const auto tmp_x = x;
      const auto tmp_y = y;
      x = -tmp_y;
      y = tmp_x;
    }
  }
  void Reflect() {
    for (auto&& [x, y] : shape) {
      x *= -1;
      y *= -1;
    }
  }
  bool InBox(const int min_x, const int max_x, const int min_y,
             const int max_y) const {
    bool ret = true;
    for (const auto& [x, y] : shape) {
      if (x < min_x) ret = false;
      if (max_x < x) ret = false;
      if (y < min_y) ret = false;
      if (max_y < y) ret = false;
    }
    return ret;
  }
};

void pentminoes() {
  const auto num_pentominoes = 13;
  const auto do_reflection = true;

  const auto x00 = Pentominoes{1, {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, -1}}};
  const auto x01 = Pentominoes{1, {{0, 0}, {1, 0}, {0, 1}, {1, 1}}};
  const auto x02 = Pentominoes{2, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}}};
  const auto x03 = Pentominoes{2, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, -1}}};
  const auto x04 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {2, 1}}};
  const auto x05 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {0, 1}, {1, 1}}};
  const auto x06 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {1, 1}, {1, 2}}};
  const auto x07 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {2, 1}}};
  const auto x08  // 解そのものに回転対称性があるため絞る目的でsym=1とする
      = Pentominoes{1, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}}};
  const auto x09 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {1, -1}, {2, 1}}};
  const auto x10 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {3, 1}}};
  const auto x11 = Pentominoes{4, {{0, 0}, {1, 0}, {2, 0}, {2, 1}, {3, 1}}};
  const auto x12 = Pentominoes{4, {{0, 0}, {1, 0}, {1, 1}, {2, 1}, {2, 2}}};
  const auto pent_list = std::vector<Pentominoes>{
      x00, x01, x02, x03, x04, x05, x06, x07, x08, x09, x10, x11, x12};

  // board は 8 x 8
  const auto num_items = num_pentominoes + 8 * 8;
  const auto coord2index = [](int x, int y) {
    return num_pentominoes + x * 8 + y;
  };
  auto option_list = std::vector<std::vector<int>>();
  auto shape_list = std::vector<Pentominoes>();
  for (auto pidx = 0; pidx < num_pentominoes; ++pidx) {
    auto p = pent_list[pidx];

    for (auto ref = 0; ref < 2; ++ref) {
      for (auto sym = 0; sym < p.symmetry; ++sym) {
        for (auto dx = -20; dx <= 20; ++dx) {
          for (auto dy = -20; dy <= 20; ++dy) {
            // 正方形は穴のため
            if (pidx == 1)
              if (not(dx == 3 and dy == 3)) continue;

            const auto tmp = p.Translated(dx, dy);
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

        p.Rotate();
      }

      if (do_reflection and
          (
              //  pidx == 3 or
              // 解には線対称性があるため、絞る目的でx03は反転しない
              pidx == 5 or pidx == 7 or pidx == 9 or pidx == 10 or pidx == 11))
        p.Reflect();
      else
        break;
    }
  }

  auto solver = ExactCoverProblemSolver(num_items, option_list);

  std::cout << "Find exact cover via daincing links" << std::endl;
  solver.SolveMultiThread(true);
  std::cout << "Done" << std::endl;

  // FIXME(masaki.ono): do_reflectionがtrueの場合、num_solutions=65となるはず
  // https://github.com/taylorjg/pentominoes-cs
  // でも今は64個しか見つかっていない
  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;

  // Print solution
  const auto print = [&](int sol) {
    const auto option_idx_list = solver.GetSolution(sol);
    auto board = Board(8, 8);
    for (auto i = 0; i < num_pentominoes; ++i) {
      const auto oidx = option_idx_list[i];
      const auto shape = shape_list[oidx];
      for (const auto& [x, y] : shape.shape) {
        board.Set(x, y, 'a' + i);
      }
    }
    std::cout << "===================" << std::endl;
    board.Print();
  };
  print(0);
  print(1);
}
int main() {
  pentminoes();
  return 0;
}