#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "common/common.h"

constexpr auto SudokuSize = 9;
constexpr auto SubSize = 3;

struct Board {
  std::vector<std::vector<int>> board;

  Board() {
    board.resize(SudokuSize);
    for (auto x = 0; x < SudokuSize; ++x) board[x].resize(SudokuSize, 0);
  }
  void Set(int x, int y, int v) { board[x][y] = v; }
  int Get(int x, int y) const { return board[x][y]; }
  void Print() const {
    for (auto y = SudokuSize - 1; y >= 0; --y) {
      for (auto x = 0; x < SudokuSize; ++x) {
        if (x != 0 and x % SubSize == 0) std::cout << " | ";
        std::cout << Get(x, y);
      }
      if (y != 0 and y % SubSize == 0) {
        std::cout << std::endl;
        for (auto i = 0; i < SubSize; ++i) {
          for (auto j = 0; j < SubSize; ++j) {
            std::cout << "-";
          }
          std::cout << "   ";
        }
      }
      std::cout << std::endl;
    }
  }
};

Board loadSudoku(const std::string& path) {
  auto ifs = std::ifstream{path};
  if (not ifs.good())
    throw std::runtime_error("Input file does not exist: " + path);
  auto ret = Board();
  auto y = SudokuSize - 1;
  while (not ifs.eof()) {
    auto s = std::string();
    std::getline(ifs, s);
    for (auto x = 0; x < SudokuSize; ++x) {
      if (s[x] == 'X') continue;
      ret.Set(x, y, s[x] - '0');
    }
    --y;
  }
  return ret;
}

void sudoku(const std::string& path, int index, bool save_solution) {
  const auto sudoku = loadSudoku(path);
  std::cout << "Problem: \n" << std::endl;
  sudoku.Print();
  std::cout << std::endl;

  auto option_handler = OptionHandler();

  for (auto x = 0; x < SudokuSize; ++x) {
    for (auto y = 0; y < SudokuSize; ++y) {
      for (auto v = 1; v <= SudokuSize; ++v) {
        const auto i1 =
            option_handler.AddItem('c', x, y);  // セルに数字を入れる
        const auto i2 =
            option_handler.AddItem('x', x, v);  // x列に数字vを入れる
        const auto i3 =
            option_handler.AddItem('y', y, v);  // y行に数字vを入れる
        const auto i4 = option_handler.AddItem('s', 3 * (x / 3) + y / 3,
                                               v);  // SubSquareに数字vを入れる
        auto option = Option();
        option.AddPrimaryItems(i1, i2, i3, i4);
        option_handler.AddOption(option);
      }
    }
  }
  // 既に入力されている数字に対応するアイテムを消去する
  for (auto x = 0; x < SudokuSize; ++x) {
    for (auto y = 0; y < SudokuSize; ++y) {
      if (const auto v = sudoku.Get(x, y); v != 0) {
        option_handler.FindItemMut('c', x, y)->SetDead();
        option_handler.FindItemMut('x', x, v)->SetDead();
        option_handler.FindItemMut('y', y, v)->SetDead();
        option_handler.FindItemMut('s', 3 * (x / 3) + y / 3, v)->SetDead();
      }
    }
  }

  const auto [num_items, option_list, compile_to_raw] =
      option_handler.XCCompile();
  auto solver = ExactCoverProblemSolver(num_items, option_list);
  runXCSolver("sudoku " + std::to_string(index), solver, save_solution);

  if (save_solution) {
    const auto option_idx_list = solver.GetSolution(0);
    auto solved_sudoku = sudoku;
    for (const auto oidx : option_idx_list) {
      const auto& option = option_handler.GetOption(compile_to_raw.at(oidx));
      const auto x = option.GetPrimaryItem(0)->Get<int>(1);
      const auto y = option.GetPrimaryItem(0)->Get<int>(2);
      const auto v = option.GetPrimaryItem(1)->Get<int>(2);
      solved_sudoku.Set(x, y, v);
    }
    std::cout << "Solution: \n" << std::endl;
    solved_sudoku.Print();
    std::cout << std::endl;
  }
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# Sudoku\n"
            << std::endl;
  std::cout << "-------------------------------" << std::endl;
  sudoku("data/sudoku/0.txt", 0, true);
  std::cout << "-------------------------------" << std::endl;
  sudoku("data/sudoku/1.txt", 1, true);
  std::cout << "-------------------------------" << std::endl;
  // Reference: https://blog.cybozu.io/entry/1692
  sudoku("data/sudoku/2.txt", 2, false);
}
