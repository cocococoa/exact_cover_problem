#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief ヌルポインタの代わりの整数
 * @note HNode の数が |NullIdx| 以上の場合、デバッグ用のプリント関数がバグる
 */
constexpr auto NullIdx = -0xFFFFFFF;

/**
 * @brief 値の列をイイ感じにプリントする
 */
template <typename Iterator>
std::string tostr(Iterator begin, Iterator end, const std::string& sep = ", ") {
  std::stringstream ss;
  while (begin != end) {
    ss << *begin++;
    if (begin != end) ss << sep;
  }
  return ss.str();
}

// Forward declaration
class ExactCoverProblemSolver;

/**
 * @brief マルチスレッドで解を探索するのを補助する関数
 * @note std::thread に渡す用の関数
 * @param solver ソルバ
 * @param initial_i アルゴリズムXのiの初期値
 * @param initial_xl アルゴリズムXのxlの初期値
 * @param save_solution 探索時に解を保存するかどうかのオプション
 * @return int 0
 */
int callSolverInMultiThreadHelper(ExactCoverProblemSolver* solver,
                                  int initial_i, int initial_xl,
                                  bool save_solution);

/**
 * @brief 完全被覆問題のソルバ
 */
class ExactCoverProblemSolver {
 public:
  friend int callSolverInMultiThreadHelper(ExactCoverProblemSolver*, int, int,
                                           bool);

 private:
  /**
   * @brief アルゴリズムXのヘッダー
   */
  struct Header {
#ifndef NDEBUG
    int idx = 0;  //!< for debug
#endif
    int llink = NullIdx;
    int rlink = NullIdx;

    std::string str() const;
  };

  /**
   * @brief アルゴリズムXのノード
   */
  struct Node {
#ifndef NDEBUG
    int idx = 0;  //!< for debug
#endif
    int fix_structure_size = 0;
    int len_or_top = NullIdx;
    int ulink = NullIdx;
    int dlink = NullIdx;

    std::string str() const;
  };

 public:
  ExactCoverProblemSolver(int i_num_items,
                          const std::vector<std::vector<int>>& i_option_list);
  ExactCoverProblemSolver(const ExactCoverProblemSolver&) = default;
  ExactCoverProblemSolver(ExactCoverProblemSolver&&) = default;

  int NumSolutions() const { return num_solutions_; }
  std::vector<int> GetSolution(int i) const;
  std::string GetPrettySolution(int i) const;
  void PrintCurrentLink() const {
    std::cout << "Print hnode" << std::endl;
    for (const auto& hnode : hnode_list_) std::cout << hnode.str() << std::endl;
    std::cout << "Print vnode" << std::endl;
    for (const auto& vnode : vnode_list_) std::cout << vnode.str() << std::endl;
  }
  void Solve(bool save_solution = false);
  void SolveMultiThread(bool save_solution = false);

 private:
  void SolveImpl(int initial_i, int initial_xl, bool start_from_x5,
                 bool save_solution);
  void Cover(int i);
  void Hide(int p);
  void UnCover(int i);
  void UnHide(int p);
  Header& HNode(int i) { return hnode_list_[i]; }
  const Header& HNode(int i) const { return hnode_list_[i]; }
  int LLink(int i) const { return HNode(i).llink; }
  int RLink(int i) const { return HNode(i).rlink; }
  Node& VNode(int i) { return vnode_list_[i]; }
  const Node& VNode(int i) const { return vnode_list_[i]; }
  int Len(int i) const { return VNode(i).len_or_top; }
  int Top(int p) const { return VNode(p).len_or_top; }
  int ULink(int p) const { return VNode(p).ulink; }
  int DLink(int p) const { return VNode(p).dlink; }
  int MRV() const;

 private:
  // Input
  const int num_items;
  const std::vector<std::vector<int>> option_list;
  std::map<int, int> vnode2idx_;
  std::vector<Header> hnode_list_;
  std::vector<Node> vnode_list_;

  // Output
  std::uint64_t num_solutions_ = 0;
  std::vector<std::vector<int>> solution_list_ = {};
};
