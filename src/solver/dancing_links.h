#pragma once

#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief ヌルポインタの代わりの整数
 * @note HNode の数が |NullIdx| 以上の場合、デバッグ用のプリント関数がバグる
 */
constexpr auto NullIdx = std::numeric_limits<int>::min();
/**
 * @brief ノードのデフォルトの色
 */
constexpr auto DefaultColor = 0;
/**
 * @brief ノードがアイテムとマッチした色を持っていることを表す数字
 */
constexpr auto CorrectColor = -1;

template <typename T>
inline std::ostream& operator<<(std::ostream& out, const std::pair<T, T>& lhs) {
  out << "(" << lhs.first << ", " << lhs.second << ")";
  return out;
}

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

#pragma region XC
/**
 * @brief 完全被覆問題のソルバ
 */
class ExactCoverProblemSolver {
 private:
  /**
   * @brief アルゴリズムXのヘッダー
   */
  struct Header {
    int llink = NullIdx;
    int rlink = NullIdx;

    std::string str() const;
  };

  /**
   * @brief アルゴリズムXのノード
   */
  struct Node {
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
  // Minimum Remaining Value
  int MRV() const;

  // Knuth's 4 operations
  void Cover(int i);
  void Hide(int p);
  void UnCover(int i);
  void UnHide(int p);

  // Utility functions
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

 private:
  // Input
  const int num_items;
  const std::vector<std::vector<int>> option_list;
  std::map<int, int> vnode2idx_;

  // Dancer
  std::vector<Header> hnode_list_;
  std::vector<Node> vnode_list_;

  // Output
  std::uint64_t num_solutions_ = 0;
  std::vector<std::vector<int>> solution_list_ = {};
};
#pragma endregion

#pragma region XCC
/**
 * @brief 色付き完全被覆問題のソルバ
 */
class ExactCoverWithColorsSolver {
 private:
  /**
   * @brief アルゴリズムXのヘッダー
   */
  struct Header {
    int llink = NullIdx;
    int rlink = NullIdx;

    std::string str() const;
  };

  /**
   * @brief アルゴリズムXのノード
   */
  struct Node {
    int len_or_top = NullIdx;
    int ulink = NullIdx;
    int dlink = NullIdx;
    int color = DefaultColor;

    std::string str() const;
  };

 public:
  ExactCoverWithColorsSolver(
      int i_num_primary_items, int i_num_secondary_items,
      const std::vector<std::vector<std::pair<int, int>>>& i_option_list);
  ExactCoverWithColorsSolver(ExactCoverWithColorsSolver&&) = default;
  ExactCoverWithColorsSolver(const ExactCoverWithColorsSolver&) = default;

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
  // Minimum Remaining Value
  int MRV() const;

  // Knuth's 8 operations
  void CoverC(int i);
  void HideC(int p);
  void UnCoverC(int i);
  void UnHideC(int p);
  void Commit(int p, int j);
  void Purify(int p);
  void UnCommit(int p, int j);
  void UnPurify(int p);

  // Utility functions
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
  int Color(int p) const { return VNode(p).color; }

 private:
  // Input
  const int num_primary_items;
  const int num_secondary_items;
  const std::vector<std::vector<std::pair<int, int>>> option_list;
  std::map<int, int> vnode2idx_;

  // Dancer
  std::vector<Header> hnode_list_;
  std::vector<Node> vnode_list_;

  // Output
  std::uint64_t num_solutions_ = 0;
  std::vector<std::vector<int>> solution_list_ = {};
};
#pragma endregion
