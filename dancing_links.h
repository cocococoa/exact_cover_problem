#pragma once

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

constexpr auto null_idx = -0xFFF;
template <typename Iterator>
std::string Tostr(Iterator begin, Iterator end, const std::string& sep = ", ") {
  std::stringstream ss;
  while (begin != end) {
    ss << *begin++;
    if (begin != end) ss << sep;
  }
  return ss.str();
}

class ExactCoverProblemSolver {
 private:
  struct HNode {
#ifndef NDEBUG
    int idx = 0;  //!< for debug
#endif
    std::string name = "X";
    int llink = null_idx;
    int rlink = null_idx;

    std::string str() const;
  };

  struct VNode {
#ifndef NDEBUG
    int idx = 0;  //!< for debug
#endif
    // TODO: len と top を共用体でまとめる
    int len = 0;
    int top = null_idx;
    int ulink = null_idx;
    int dlink = null_idx;

    std::string str() const;
  };

 public:
  ExactCoverProblemSolver(int i_num_items,
                          const std::vector<std::vector<int>>& i_option_list);
  int NumSolutions() const { return num_solutions_; }
  std::vector<int> GetSolution(int i) const;
  std::string GetPrettySolution(int i) const;
  void PrintCurrentLink() const {
    std::cout << "Print hnode" << std::endl;
    for (const auto& hnode : hnode_list_) std::cout << hnode.str() << std::endl;
    std::cout << "Print vnode" << std::endl;
    for (const auto& vnode : vnode_list_) std::cout << vnode.str() << std::endl;
  }
  void Solve(bool count_mode = false);

 private:
  void cover(int i);
  void hide(int p);
  void uncover(int i);
  void unhide(int p);
  HNode& hnode(int i) { return hnode_list_[i]; }
  const HNode& hnode(int i) const { return hnode_list_[i]; }
  int llink(int i) const { return hnode(i).llink; }
  int rlink(int i) const { return hnode(i).rlink; }
  VNode& vnode(int i) { return vnode_list_[i]; }
  const VNode& vnode(int i) const { return vnode_list_[i]; }
  int len(int i) const { return vnode(i).len; }
  int top(int p) const { return vnode(p).top; }
  int ulink(int p) const { return vnode(p).ulink; }
  int dlink(int p) const { return vnode(p).dlink; }
  int MRV() const;

 private:
  // Input
  const int num_items;
  const std::vector<std::vector<int>> option_list;
  std::map<int, int> vnode2idx_;
  std::vector<HNode> hnode_list_;
  std::vector<VNode> vnode_list_;

  // Output
  std::uint64_t num_solutions_ = 0;
  std::vector<std::vector<int>> solution_list_ = {};
};
