#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "common.h"

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

class OptionManager {
 public:
  OptionManager() {}
  void SetItemTypes(int num_types) { num_types_ = num_types; }
  void AddOption(const std::vector<int>& option) {
    if ((int)option.size() != num_types_)
      throw std::runtime_error(
          "Option size error: " + std::to_string(option.size()) +
          ", expected: " + std::to_string(num_types_));

    raw_option_list_.push_back(option);
  }
  void DeleteItem(int type, int item) {
    if (type >= num_types_) throw std::runtime_error("Item type error");
    delete_option_list_.emplace_back(type, item);
  }
  const std::vector<int>& GetRawOption(int compile_option_idx) const {
    return raw_option_list_[compile_to_raw_.at(compile_option_idx)];
  }
  std::pair<int, std::vector<std::vector<int>>> Compile() const {
    auto item_list = std::vector<std::set<int>>(num_types_);
    for (const auto& option : raw_option_list_) {
      for (auto type = 0; type < num_types_; ++type) {
        item_list[type].insert(option[type]);
      }
    }
    for (const auto& [type, item] : delete_option_list_) {
      item_list[type].erase(item);
    }

    auto num_items = 0;
    auto option_list = std::vector<std::vector<int>>();
    auto mapper = std::map<std::pair<int, int>, int>();
    for (auto type = 0; type < num_types_; ++type) {
      for (const auto item : item_list[type]) {
        mapper[{type, item}] = num_items++;
      }
    }
    for (auto raw_option_idx = 0; raw_option_idx < (int)raw_option_list_.size();
         raw_option_idx++) {
      const auto& raw_option = raw_option_list_[raw_option_idx];
      auto option = std::vector<int>();
      for (auto type = 0; type < num_types_; ++type) {
        const auto key = std::make_pair(type, raw_option[type]);
        if (mapper.count(key) > 0)
          option.push_back(mapper[key]);
        else
          break;
      }
      if ((int)option.size() == num_types_) {
        option_list.push_back(option);
        compile_to_raw_[(int)option_list.size() - 1] = raw_option_idx;
      }
    }
    return {num_items, option_list};
  }

 private:
  int num_types_ = 1;
  std::vector<std::vector<int>> raw_option_list_ = {};
  std::vector<std::pair<int, int>> delete_option_list_ = {};
  mutable std::map<int, int> compile_to_raw_ = {};
};

void sudoku(const std::string& path, int index) {
  const auto sudoku = loadSudoku(path);
  std::cout << "Problem: \n" << std::endl;
  sudoku.Print();
  std::cout << std::endl;

  auto manager = OptionManager();
  manager.SetItemTypes(4);
  for (auto x = 0; x < SudokuSize; ++x) {
    for (auto y = 0; y < SudokuSize; ++y) {
      for (auto v = 1; v <= SudokuSize; ++v) {
        manager.AddOption({
            SudokuSize * x + y,      // セルに数字を入れる
            SudokuSize * x + v - 1,  // x列に数字vを入れる
            SudokuSize * y + v - 1,  // y行に数字vを入れる
            SudokuSize * (3 * (x / 3) + y / 3) + v -
                1  // SubSquareに数字vを入れる
        });
      }
    }
  }
  // 既に入力されている数字に対応するアイテムを消去する
  for (auto x = 0; x < SudokuSize; ++x) {
    for (auto y = 0; y < SudokuSize; ++y) {
      if (const auto v = sudoku.Get(x, y); v != 0) {
        manager.DeleteItem(0, SudokuSize * x + y);
        manager.DeleteItem(1, SudokuSize * x + v - 1);
        manager.DeleteItem(2, SudokuSize * y + v - 1);
        manager.DeleteItem(3, SudokuSize * (3 * (x / 3) + y / 3) + v - 1);
      }
    }
  }

  const auto [num_items, option_list] = manager.Compile();
  auto solver = ExactCoverProblemSolver(num_items, option_list);
  runXCSolver("sudoku " + std::to_string(index), solver, true);

  const auto option_idx_list = solver.GetSolution(0);
  auto solved_sudoku = sudoku;
  for (const auto comiled_option_idx : option_idx_list) {
    const auto& raw_option = manager.GetRawOption(comiled_option_idx);
    const auto x = raw_option[0] / SudokuSize;
    const auto y = raw_option[0] % SudokuSize;
    const auto v = raw_option[1] % SudokuSize + 1;
    solved_sudoku.Set(x, y, v);
  }
  std::cout << "Solution: \n" << std::endl;
  solved_sudoku.Print();
  std::cout << std::endl;
}

int main() {
  std::cout << "-----------------------------------------------------------\n"
            << "# Sudoku\n"
            << std::endl;
  std::cout << "-------------------------------" << std::endl;
  sudoku("data/sudoku/0.txt", 0);
  std::cout << "-------------------------------" << std::endl;
  sudoku("data/sudoku/1.txt", 1);
}
