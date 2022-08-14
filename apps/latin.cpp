#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "common.h"

constexpr auto LatinSize = 10;

template <typename Iterator>
bool isIotaArray(Iterator begin, Iterator end, int value) {
  while (begin != end) {
    if (*begin != value) return false;
    begin++;
    value++;
  }
  return true;
}

struct Transverse {
  std::array<std::uint8_t, LatinSize> t;
};

struct Latin {
  std::array<std::array<std::uint8_t, LatinSize>, LatinSize> latin;

  void Set(int x, int y, std::uint8_t v) { latin[x][y] = v; }
  std::uint8_t Get(int x, int y) const { return latin[x][y]; }
  int Size() const { return LatinSize; }
  void Print() const {
    for (auto y = 0; y < LatinSize; ++y) {
      for (auto x = 0; x < LatinSize; ++x) {
        std::cout << (int)Get(x, y);
      }
      std::cout << std::endl;
    }
  }
  bool IsTransverse(const Transverse& t) const {
    auto index_list = std::vector<int>();
    index_list.reserve(LatinSize);
    for (auto x = 0; x < LatinSize; ++x)
      for (auto y = 0; y < LatinSize; ++y)
        if (t.t[x] == Get(x, y)) index_list.push_back(y);
    std::sort(index_list.begin(), index_list.end());
    return isIotaArray(index_list.begin(), index_list.end(), 0);
  }
};

Latin loadLatin(const std::string& path) {
  auto ifs = std::ifstream{path};
  if (not ifs.good())
    throw std::runtime_error("Input file does not exist: " + path);
  auto ret = Latin();
  auto y = 0;
  while (not ifs.eof()) {
    auto s = std::string();
    std::getline(ifs, s);
    for (auto x = 0; x < LatinSize; ++x) {
      ret.Set(x, y, s[x] - '0');
    }
    ++y;
  }
  return ret;
}

std::vector<Transverse> calcTransverse(const Latin& latin) {
  auto ret = std::vector<Transverse>();
  auto t = Transverse();
  std::iota(t.t.begin(), t.t.end(), std::uint8_t{0});
  do {
    if (latin.IsTransverse(t)) ret.push_back(t);
  } while (std::next_permutation(t.t.begin(), t.t.end()));
  return ret;
}

void searchLatin(const std::string& path) {
  const auto latin = loadLatin(path);
  std::cout << "-------------------------------" << std::endl;
  latin.Print();
  auto transverse_list = calcTransverse(latin);
  std::cout << "Num transverses: " << transverse_list.size() << std::endl;

  const auto num_items = LatinSize * LatinSize;
  auto option_list = std::vector<std::vector<int>>();
  for (const auto& t : transverse_list) {
    auto option = std::vector<int>(LatinSize);
    for (auto x = 0; x < LatinSize; ++x) {
      for (auto y = 0; y < LatinSize; ++y)
        if (t.t[x] == latin.Get(x, y)) option[x] = x * LatinSize + y;
    }

    option_list.emplace_back(option);
  }

  auto solver = ExactCoverProblemSolver(num_items, option_list);
  runSolver(solver, false);
}

int main() {
  searchLatin("data/latin/0.txt");
  searchLatin("data/latin/1.txt");
  searchLatin("data/latin/2.txt");
  searchLatin("data/latin/3.txt");
  searchLatin("data/latin/4.txt");  // 12分くらいかかる
}