#include "solver/dancing_links.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <thread>

std::string ExactCoverProblemSolver::HNode::str() const {
  std::stringstream ss;
#ifndef NDEBUG
  ss << "i: " << idx << ", ";
#endif
  ss << "NAME(i): " << name << ", LLINK(i): " << llink
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
    if (top == NullIdx)
      ss << "--";
    else
      ss << std::setw(2) << top;

    ss << ", ULINK(x): ";
    if (ulink == NullIdx)
      ss << "--";
    else
      ss << std::setw(2) << ulink;

    ss << ", DLINK(x): ";
    if (dlink == NullIdx)
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
std::vector<int> ExactCoverProblemSolver::GetSolution(int i) const {
  auto ret = std::vector<int>();
  const auto& sol = solution_list_[i];
  for (auto j = 0; j < (int)sol.size(); ++j) {
    const auto oidx = vnode2idx_.at(sol[j]);
    ret.push_back(oidx);
  }
  return ret;
}
std::string ExactCoverProblemSolver::GetPrettySolution(int i) const {
  const auto sep = ", ";
  auto ret = std::string("{");
  const auto sol = GetSolution(i);
  for (auto j = 0; j < (int)sol.size(); ++j) {
    const auto& option = option_list[sol[j]];
    ret += "{" + tostr(option.begin(), option.end(), sep) + "}";
    if (j + 1 != (int)sol.size()) ret += sep;
  }
  ret += "}";
  return ret;
}
void ExactCoverProblemSolver::Solve(bool save_solution) {
  SolveImpl(0, 0, false, save_solution);
}
int callSolverInMultiThreadHelper(ExactCoverProblemSolver* solver,
                                  int initial_i, int initial_xl,
                                  bool save_solution) {
  solver->SolveImpl(initial_i, initial_xl, true, save_solution);
  return 0;
}
void ExactCoverProblemSolver::SolveMultiThread(bool save_solution) {
  // Select i
  // TODO(masaki.ono): initial_i の決め方を工夫する
  const auto initial_i = 1;
  Cover(initial_i);

  // Create copies of *this
  const auto num_copies = Len(initial_i);
  std::cout << "Num copies: " << num_copies << std::endl;
  auto initial_xl_list = std::vector<int>();
  for (auto xl = DLink(initial_i); xl != initial_i; xl = DLink(xl)) {
    initial_xl_list.push_back(xl);
  }
  auto solver_list = std::vector<ExactCoverProblemSolver>(num_copies, *this);

  // Run solvers
  auto thread_list = std::vector<std::thread>();
  for (auto c = 0; c < num_copies; ++c) {
    thread_list.emplace_back(std::thread(callSolverInMultiThreadHelper,
                                         &solver_list[c], initial_i,
                                         initial_xl_list[c], save_solution));
  }
  for (auto&& thread : thread_list) thread.join();

  // Collect Results
  for (const auto& solver : solver_list) {
    num_solutions_ += solver.NumSolutions();
    if (save_solution) {
      std::copy(solver.solution_list_.begin(), solver.solution_list_.end(),
                std::back_inserter(solution_list_));
    }
  }
}
void ExactCoverProblemSolver::SolveImpl(int initial_i, int initial_xl,
                                        bool start_from_x5,
                                        bool save_solution) {
  auto sol = std::vector<int>();
  sol.reserve(num_items);
  auto i = initial_i;
  auto xl = initial_xl;
  auto level = 0;
  const auto leave_level = start_from_x5 ? 1 : 0;
  if (start_from_x5) goto X5;

X2:
  // std::cout << "X2: Enter level l: " << level << std::endl;
  if (RLink(0) == 0) {
    // Hence all items have been covered
    num_solutions_++;
    if (save_solution) {
      solution_list_.emplace_back(std::vector<int>(sol));
    }
    goto X8;
  }

  // X3:
  // i = RLink(0);
  i = MRV();
  // std::cout << "X3: Choose i: " << i << std::endl;

  // X4:
  // std::cout << "X4: Cover i: " << i << std::endl;
  Cover(i);
  xl = DLink(i);

X5:
  // std::cout << "X5: Try xl: " << xl << std::endl;
  if (xl == i) {
    // std::cout << "We've tried all options for i: " << i << std::endl;
    goto X7;
  } else {
    // This covers the items \neq i in the option that contains xl
    auto p = xl + 1;
    while (p != xl) {
      const auto j = Top(p);
      if (j <= 0) {
        p = ULink(p);
      } else {
        Cover(j);
        p = p + 1;
      }
    }
    level++;
    sol.emplace_back(xl);
    goto X2;
  }

X6:
  // std::cout << "X6: Try again: " << xl << std::endl;
  {
    // This uncovers the items \neq i in the option that contains xl
    auto p = xl - 1;
    while (p != xl) {
      const auto j = Top(p);
      if (j <= 0) {
        p = DLink(p);
      } else {
        UnCover(j);
        p = p - 1;
      }
    }
  }
  i = Top(xl);
  xl = DLink(xl);
  goto X5;

X7:
  // std::cout << "X7: Backtrack" << std::endl;
  UnCover(i);

X8:
  // std::cout << "X8: Leave level l: " << level << std::endl;
  if (level == leave_level) {
    return;
  } else {
    level--;
    xl = sol.back();
    sol.pop_back();
    goto X6;
  }
}
void ExactCoverProblemSolver::Cover(int i) {
  auto p = DLink(i);
  while (p != i) {
    Hide(p);
    p = DLink(p);
  }
  const auto l = LLink(i);
  const auto r = RLink(i);
  Hnode(l).rlink = r;
  Hnode(r).llink = l;
}
void ExactCoverProblemSolver::Hide(int p) {
  auto q = p + 1;
  while (q != p) {
    const auto x = Top(q);
    const auto u = ULink(q);
    const auto d = DLink(q);
    if (x <= 0) {
      // q was a spacer
      q = u;
    } else {
      Vnode(u).dlink = d;
      Vnode(d).ulink = u;
      Vnode(x).len -= 1;
      q += 1;
    }
  }
}
void ExactCoverProblemSolver::UnCover(int i) {
  const auto l = LLink(i);
  const auto r = RLink(i);
  Hnode(l).rlink = i;
  Hnode(r).llink = i;
  auto p = ULink(i);
  while (p != i) {
    UnHide(p);
    p = ULink(p);
  }
}
void ExactCoverProblemSolver::UnHide(int p) {
  auto q = p - 1;
  while (q != p) {
    const auto x = Top(q);
    const auto u = ULink(q);
    const auto d = DLink(q);
    if (x <= 0) {
      // q was a spacer
      q = d;
    } else {
      Vnode(u).dlink = q;
      Vnode(d).ulink = q;
      Vnode(x).len += 1;
      q -= 1;
    }
  }
}
int ExactCoverProblemSolver::MRV() const {
  // Minimum remaining values
  auto i = RLink(0);
  auto ret = i;
  auto min_len = (int)option_list.size() + 100;
  while (i != 0) {
    const auto l = Len(i);
    if (l < min_len) {
      min_len = l;
      ret = i;
    }
    i = RLink(i);
  }
  return ret;
}
