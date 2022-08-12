#include "dancing_links.h"

#include <algorithm>
#include <iomanip>
#include <numeric>

std::string ExactCoverProblemSolver::HNode::str() const {
  std::stringstream ss;
  ss
#ifndef NDEBUG
      << "i: " << idx << ", "
#endif
      << "NAME(i): " << name << ", LLINK(i): " << llink
      << ", RLINK(i): " << rlink;
  return ss.str();
}
std::string ExactCoverProblemSolver::VNode::str() const {
  std::stringstream ss;
  if (len == 0) {
#ifndef NDEBUG
    ss << "x: ";
    ss << std::setw(2) << idx;
    ss << ", ";
#endif
    ss << "TOP(x): ";
    if (top == null_idx)
      ss << "--";
    else
      ss << std::setw(2) << top;
    ss << ", ULINK(x): ";
    if (ulink == null_idx)
      ss << "--";
    else
      ss << std::setw(2) << ulink;
    ss << ", DLINK(x): ";
    if (dlink == null_idx)
      ss << "--";
    else
      ss << std::setw(2) << dlink;
  } else {
#ifndef NDEBUG
    ss << "x: ";
    ss << std::setw(2) << idx;
    ss << ", ";
#endif
    ss << "LEN(x): ";
    ss << std::setw(2) << len;
    ss << ", ULINK(x): ";
    ss << std::setw(2) << ulink;
    ss << ", DLINK(x): ";
    ss << std::setw(2) << dlink;
  }
  return ss.str();
}
ExactCoverProblemSolver::ExactCoverProblemSolver(
    int i_num_items, const std::vector<std::vector<int>>& i_option_list)
    : num_items(i_num_items), option_list(i_option_list) {
  const auto num_options = (int)option_list.size();
  const auto num_total_items = std::accumulate(
      option_list.begin(), option_list.end(), 0,
      [](auto acc, const auto& x) { return acc + (int)x.size(); });
  hnode_list_.clear();
  vnode_list_.clear();
  hnode_list_.resize(1 + num_items);
  vnode_list_.resize(1 + num_items + num_options + num_total_items + 1);
  std::cout << "hnode size: " << hnode_list_.size() << std::endl;
  std::cout << "vnode size: " << vnode_list_.size() << std::endl;

#ifndef NDEBUG
  // Set index for debug
  for (auto i = 0; i < 1 + num_items; ++i) hnode_list_[i].idx = i;
  for (auto i = 0; i < 1 + num_items + num_options + num_total_items + 1; ++i)
    vnode_list_[i].idx = i;
#endif

  // Initialize hnode
  for (auto i = 0; i < num_items; ++i) {
    hnode_list_[i].rlink = i + 1;
    hnode_list_[i + 1].llink = i;
    hnode_list_[i + 1].name = 'a' + i;
  }
  hnode_list_[0].llink = num_items;
  hnode_list_[num_items].rlink = 0;

  // Initialize vnode
  auto item_idx = 0;
  auto idx = 1 + num_items;
  for (auto oidx = 0; oidx < (int)option_list.size(); ++oidx) {
    const auto& option = option_list[oidx];
    const auto left_idx = idx;
    vnode_list_[left_idx].top = -item_idx++;
    idx++;
    for (const auto item : option) {
      vnode2idx_[idx] = oidx;
      if (vnode_list_[1 + item].len == 0) {
        vnode_list_[1 + item].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
        vnode_list_[idx].ulink = 1 + item;
      } else {
        const auto ulink = vnode_list_[1 + item].ulink;
        vnode_list_[idx].ulink = ulink;
        vnode_list_[ulink].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
      }
      vnode_list_[1 + item].len++;
      vnode_list_[idx].dlink = 1 + item;
      vnode_list_[idx].top = 1 + item;

      idx++;
    }
    const auto right_idx = idx;
    vnode_list_[left_idx].dlink = right_idx - 1;
    vnode_list_[right_idx].ulink = left_idx + 1;
    vnode_list_[right_idx].top = -item_idx;
  }
}
std::string ExactCoverProblemSolver::GetPrettySolution(int i) const {
  const auto sep = ", ";
  auto ret = std::string("{");
  const auto& sol = solution_list_[i];
  for (auto j = 0; j < (int)sol.size(); ++j) {
    const auto oidx = vnode2idx_.at(sol[j]);
    const auto& option = option_list[oidx];
    ret += "{" + Tostr(option.begin(), option.end(), sep) + "}";
    if (j + 1 != (int)sol.size()) ret += sep;
  }
  ret += "}";
  return ret;
}
void ExactCoverProblemSolver::Solve(bool count_mode) {
  auto sol = std::vector<int>();
  sol.reserve(num_items);
  auto i = 0;
  auto xl = 0;
  auto level = 0;

X2:
  // std::cout << "X2: Enter level l: " << level << std::endl;
  if (rlink(0) == 0) {
    // Hence all items have been covered
    // std::cout << "Found solution!" << std::endl;
    num_solutions_++;
    if (not count_mode) {
      solution_list_.emplace_back(std::vector<int>(sol));
    }
    goto X8;
  }

  // X3:
  // i = rlink(0);
  i = MRV();
  // std::cout << "X3: Choose i: " << i << std::endl;

  // X4:
  // std::cout << "X4: Cover i: " << i << std::endl;
  cover(i);
  xl = dlink(i);

X5:
  // std::cout << "X5: Try xl: " << xl << std::endl;
  if (xl == i) {
    // std::cout << "We've tried all options for i: " << i << std::endl;
    goto X7;
  } else {
    // This covers the items \neq i in the option that contains xl
    auto p = xl + 1;
    while (p != xl) {
      const auto j = top(p);
      if (j <= 0) {
        p = ulink(p);
      } else {
        cover(j);
        p = p + 1;
      }
    }
    level++;
    sol.emplace_back(xl);
    // std::cout << "[Push] Current solution: " << Tostr(sol.begin(),
    // sol.end()) << std::endl;
    goto X2;
  }

X6:
  // std::cout << "X6: Try again: " << xl << std::endl;
  {
    // This uncovers the items \neq i in the option that contains xl
    auto p = xl - 1;
    while (p != xl) {
      const auto j = top(p);
      if (j <= 0) {
        p = dlink(p);
      } else {
        uncover(j);
        p = p - 1;
      }
    }
  }
  i = top(xl);
  xl = dlink(xl);
  goto X5;

X7:
  // std::cout << "X7: Backtrack" << std::endl;
  uncover(i);

X8:
  // std::cout << "X8: Leave level l: " << level << std::endl;
  if (level == 0) {
    return;
  } else {
    level--;
    xl = sol.back();
    sol.pop_back();
    // std::cout << "[Pop] Current solution: " << Tostr(sol.begin(),
    // sol.end()) << std::endl;
    goto X6;
  }
}
void ExactCoverProblemSolver::cover(int i) {
  // std::cout << "Cover: " << i << std::endl;
  auto p = dlink(i);
  while (p != i) {
    hide(p);
    p = dlink(p);
  }
  const auto l = llink(i);
  const auto r = rlink(i);
  hnode(l).rlink = r;
  hnode(r).llink = l;
}
void ExactCoverProblemSolver::hide(int p) {
  // std::cout << "Hide: " << p << std::endl;
  auto q = p + 1;
  while (q != p) {
    const auto x = top(q);
    const auto u = ulink(q);
    const auto d = dlink(q);
    if (x <= 0) {
      // q was a spacer
      q = u;
    } else {
      vnode(u).dlink = d;
      vnode(d).ulink = u;
      vnode(x).len -= 1;
      q += 1;
    }
  }
}
void ExactCoverProblemSolver::uncover(int i) {
  // std::cout << "Uncover: " << i << std::endl;
  const auto l = llink(i);
  const auto r = rlink(i);
  hnode(l).rlink = i;
  hnode(r).llink = i;
  auto p = ulink(i);
  while (p != i) {
    unhide(p);
    p = ulink(p);
  }
}
void ExactCoverProblemSolver::unhide(int p) {
  // std::cout << "Unhide: " << p << std::endl;
  auto q = p - 1;
  while (q != p) {
    const auto x = top(q);
    const auto u = ulink(q);
    const auto d = dlink(q);
    if (x <= 0) {
      // q was a spacer
      q = d;
    } else {
      vnode(u).dlink = q;
      vnode(d).ulink = q;
      vnode(x).len += 1;
      q -= 1;
    }
  }
}
int ExactCoverProblemSolver::MRV() const {
  // Minimum remaining values
  auto i = rlink(0);
  auto ret = i;
  auto min_len = (int)option_list.size() + 100;
  while (i != 0) {
    const auto l = len(i);
    if (l < min_len) {
      min_len = l;
      ret = i;
    }
    i = rlink(i);
  }
  return ret;
}