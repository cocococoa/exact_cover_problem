#include "solver/dancing_links.h"

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <thread>

#pragma region XC
std::string ExactCoverProblemSolver::Header::str() const {
  std::stringstream ss;
  ss << "LLINK(i): " << llink << ", RLINK(i): " << rlink;
  return ss.str();
}

std::string ExactCoverProblemSolver::Node::str() const {
  std::stringstream ss;
  ss << "LEN OR TOP(x): ";
  if (len_or_top == NullIdx)
    ss << "--";
  else
    ss << std::setw(2) << len_or_top;

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

  // Initialize hnode
  for (auto i = 0; i < num_items; ++i) {
    hnode_list_[i].rlink = i + 1;
    hnode_list_[i + 1].llink = i;
  }
  hnode_list_[0].llink = num_items;
  hnode_list_[num_items].rlink = 0;

  // Initialize vnode
  auto item_idx = 0;
  auto idx = 1 + num_items;
  for (auto j = 0; j < 1 + num_items; ++j) vnode_list_[j].len_or_top = 0;
  for (auto oidx = 0; oidx < (int)option_list.size(); ++oidx) {
    const auto& option = option_list[oidx];
    const auto left_idx = idx;
    vnode_list_[left_idx].len_or_top = -item_idx++;
    idx++;
    for (const auto item : option) {
      vnode2idx_[idx] = oidx;
      if (vnode_list_[1 + item].len_or_top == 0) {
        vnode_list_[1 + item].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
        vnode_list_[idx].ulink = 1 + item;
      } else {
        const auto ulink = vnode_list_[1 + item].ulink;
        vnode_list_[idx].ulink = ulink;
        vnode_list_[ulink].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
      }
      vnode_list_[1 + item].len_or_top++;
      vnode_list_[idx].dlink = 1 + item;
      vnode_list_[idx].len_or_top = 1 + item;

      idx++;
    }
    const auto right_idx = idx;
    vnode_list_[left_idx].dlink = right_idx - 1;
    vnode_list_[right_idx].ulink = left_idx + 1;
    vnode_list_[right_idx].len_or_top = -item_idx;
  }
}

std::vector<int> ExactCoverProblemSolver::GetSolution(int i) const {
  auto ret = std::vector<int>();
  const auto& sol = solution_list_[i];
  for (auto j = 0; j < (int)sol.size(); ++j) {
    const auto oidx = vnode2idx_.at(sol[j]);
    ret.push_back(oidx);
  }
  std::sort(ret.begin(), ret.end());
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

void ExactCoverProblemSolver::SolveMultiThread(bool save_solution) {
  // Select i
  // TODO(masaki.ono): initial_i ???????????????????????????
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
    thread_list.emplace_back(std::thread(
        [](ExactCoverProblemSolver* solver, int initial_i, int initial_xl,
           bool save_solution) {
          solver->SolveImpl(initial_i, initial_xl, true, save_solution);
        },
        &solver_list[c], initial_i, initial_xl_list[c], save_solution));
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

  const auto leave_level = start_from_x5 ? 1 : 0;
  auto level = 0;

  auto i = initial_i;
  auto xl = initial_xl;
  if (not start_from_x5) {
    i = MRV();
    Cover(i);
    xl = DLink(i);
  }

  while (true) {
    while (xl != i) {
      // X5
      {
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
      }
      level++;
      sol.emplace_back(xl);

      // X2
      if (RLink(0) == 0) {
        num_solutions_++;
        if (save_solution) solution_list_.emplace_back(sol);
        break;
      }

      // X3
      // TODO(masaki.ono): i ??????????????????????????????
      // i = RLink(0);
      i = MRV();

      // X4
      Cover(i);
      xl = DLink(i);
    }
    // X7
    if (xl == i) UnCover(i);

    // X8
    if (level == leave_level) [[unlikely]]
      break;
    level--;
    xl = sol.back();
    sol.pop_back();

    // X6
    {
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

void ExactCoverProblemSolver::Cover(int i) {
  auto p = DLink(i);
  while (p != i) {
    Hide(p);
    p = DLink(p);
  }
  const auto l = LLink(i);
  const auto r = RLink(i);
  HNode(l).rlink = r;
  HNode(r).llink = l;
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
      VNode(u).dlink = d;
      VNode(d).ulink = u;
      VNode(x).len_or_top -= 1;
      q += 1;
    }
  }
}

void ExactCoverProblemSolver::UnCover(int i) {
  const auto l = LLink(i);
  const auto r = RLink(i);
  HNode(l).rlink = i;
  HNode(r).llink = i;
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
      VNode(u).dlink = q;
      VNode(d).ulink = q;
      VNode(x).len_or_top += 1;
      q -= 1;
    }
  }
}
#pragma endregion

#pragma region XCC
std::string ExactCoverWithColorsSolver::Header::str() const {
  std::stringstream ss;
  ss << "LLINK(i): " << llink << ", RLINK(i): " << rlink;
  return ss.str();
}

std::string ExactCoverWithColorsSolver::Node::str() const {
  std::stringstream ss;
  ss << "LEN OR TOP(x): ";
  if (len_or_top == NullIdx)
    ss << "--";
  else
    ss << std::setw(2) << len_or_top;

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

  ss << ", COLOR(x): ";
  ss << std::setw(2) << color;

  return ss.str();
}

ExactCoverWithColorsSolver::ExactCoverWithColorsSolver(
    int i_num_primary_items, int i_num_secondary_items,
    const std::vector<std::vector<std::pair<int, int>>>& i_option_list)
    : num_primary_items(i_num_primary_items),
      num_secondary_items(i_num_secondary_items),
      option_list(i_option_list) {
  const auto num_items = num_primary_items + num_secondary_items;
  const auto num_options = (int)option_list.size();
  const auto num_total_items = std::accumulate(
      option_list.begin(), option_list.end(), 0,
      [](auto acc, const auto& x) { return acc + (int)x.size(); });
  hnode_list_.clear();
  vnode_list_.clear();
  hnode_list_.resize(1 + num_items + 1);
  vnode_list_.resize(1 + num_items + num_options + num_total_items + 1);
  std::cout << "hnode size: " << hnode_list_.size() << std::endl;
  std::cout << "vnode size: " << vnode_list_.size() << std::endl;

  // Initialize hnode
  for (auto i = 0; i < num_primary_items; ++i) {
    hnode_list_[i].rlink = i + 1;
    hnode_list_[i + 1].llink = i;
  }
  hnode_list_[0].llink = num_primary_items;
  hnode_list_[num_primary_items].rlink = 0;
  for (auto j = 1; j <= num_secondary_items; ++j) {
    hnode_list_[num_primary_items + j].rlink = num_primary_items + j + 1;
    hnode_list_[num_primary_items + j + 1].llink = num_primary_items + j;
  }
  hnode_list_[num_primary_items + 1].llink = num_items + 1;
  hnode_list_[num_items + 1].rlink = num_primary_items + 1;

  // Initialize vnode
  auto item_idx = 0;
  auto idx = 1 + num_items;
  for (auto j = 0; j < 1 + num_items; ++j) vnode_list_[j].len_or_top = 0;
  for (auto oidx = 0; oidx < (int)option_list.size(); ++oidx) {
    const auto& option = option_list[oidx];
    const auto left_idx = idx;
    vnode_list_[left_idx].len_or_top = -item_idx++;
    idx++;
    for (const auto& [item, color] : option) {
      vnode2idx_[idx] = oidx;
      if (vnode_list_[1 + item].len_or_top == 0) {
        vnode_list_[1 + item].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
        vnode_list_[idx].ulink = 1 + item;
      } else {
        const auto ulink = vnode_list_[1 + item].ulink;
        vnode_list_[idx].ulink = ulink;
        vnode_list_[ulink].dlink = idx;
        vnode_list_[1 + item].ulink = idx;
      }
      vnode_list_[1 + item].len_or_top++;
      vnode_list_[idx].dlink = 1 + item;
      vnode_list_[idx].len_or_top = 1 + item;
      vnode_list_[idx].color = color;

      idx++;
    }
    const auto right_idx = idx;
    vnode_list_[left_idx].dlink = right_idx - 1;
    vnode_list_[right_idx].ulink = left_idx + 1;
    vnode_list_[right_idx].len_or_top = -item_idx;
  }
}

std::vector<int> ExactCoverWithColorsSolver::GetSolution(int i) const {
  auto ret = std::vector<int>();
  const auto& sol = solution_list_[i];
  for (auto j = 0; j < (int)sol.size(); ++j) {
    const auto oidx = vnode2idx_.at(sol[j]);
    ret.push_back(oidx);
  }
  std::sort(ret.begin(), ret.end());
  return ret;
}

std::string ExactCoverWithColorsSolver::GetPrettySolution(int i) const {
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

void ExactCoverWithColorsSolver::Solve(bool save_solution) {
  SolveImpl(0, 0, false, save_solution);
}

void ExactCoverWithColorsSolver::SolveMultiThread(bool save_solution) {
  // Select i
  // TODO(masaki.ono): initial_i ???????????????????????????
  const auto initial_i = 1;
  CoverC(initial_i);

  // Create copies of *this
  const auto num_copies = Len(initial_i);
  std::cout << "Num copies: " << num_copies << std::endl;
  auto initial_xl_list = std::vector<int>();
  for (auto xl = DLink(initial_i); xl != initial_i; xl = DLink(xl)) {
    initial_xl_list.push_back(xl);
  }
  auto solver_list = std::vector<ExactCoverWithColorsSolver>(num_copies, *this);

  // Run solvers
  auto thread_list = std::vector<std::thread>();
  for (auto c = 0; c < num_copies; ++c) {
    thread_list.emplace_back(std::thread(
        [](ExactCoverWithColorsSolver* solver, int initial_i, int initial_xl,
           bool save_solution) {
          solver->SolveImpl(initial_i, initial_xl, true, save_solution);
        },
        &solver_list[c], initial_i, initial_xl_list[c], save_solution));
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

void ExactCoverWithColorsSolver::SolveImpl(int initial_i, int initial_xl,
                                           bool start_from_x5,
                                           bool save_solution) {
  auto sol = std::vector<int>();
  sol.reserve(num_primary_items + num_secondary_items);

  const auto leave_level = start_from_x5 ? 1 : 0;
  auto level = 0;

  auto i = initial_i;
  auto xl = initial_xl;
  if (not start_from_x5) {
    i = MRV();
    CoverC(i);
    xl = DLink(i);
  }

  while (true) {
    while (xl != i) {
      // C5
      {
        auto p = xl + 1;
        while (p != xl) {
          const auto j = Top(p);
          if (j <= 0) {
            p = ULink(p);
          } else {
            Commit(p, j);
            p = p + 1;
          }
        }
      }
      level++;
      sol.emplace_back(xl);

      // C2
      if (RLink(0) == 0) {
        num_solutions_++;
        if (save_solution) solution_list_.emplace_back(sol);
        break;
      }

      // C3
      // TODO(masaki.ono): i ??????????????????????????????
      // i = RLink(0);
      i = MRV();

      // C4
      CoverC(i);
      xl = DLink(i);
    }
    // C7
    if (xl == i) UnCoverC(i);

    // C8
    if (level == leave_level) [[unlikely]]
      break;
    level--;
    xl = sol.back();
    sol.pop_back();

    // C6
    {
      auto p = xl - 1;
      while (p != xl) {
        const auto j = Top(p);
        if (j <= 0) {
          p = DLink(p);
        } else {
          UnCommit(p, j);
          p = p - 1;
        }
      }
    }
    i = Top(xl);
    xl = DLink(xl);
  }
}

int ExactCoverWithColorsSolver::MRV() const {
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

void ExactCoverWithColorsSolver::CoverC(int i) {
  auto p = DLink(i);
  while (p != i) {
    HideC(p);
    p = DLink(p);
  }
  const auto l = LLink(i);
  const auto r = RLink(i);
  HNode(l).rlink = r;
  HNode(r).llink = l;
}

void ExactCoverWithColorsSolver::HideC(int p) {
  auto q = p + 1;
  while (q != p) {
    const auto x = Top(q);
    const auto u = ULink(q);
    const auto d = DLink(q);
    const auto c = Color(q);
    if (x <= 0) {
      // q was a spacer
      q = u;
    } else if (c == CorrectColor) {
      q += 1;
    } else {
      VNode(u).dlink = d;
      VNode(d).ulink = u;
      VNode(x).len_or_top -= 1;
      q += 1;
    }
  }
}

void ExactCoverWithColorsSolver::UnCoverC(int i) {
  const auto l = LLink(i);
  const auto r = RLink(i);
  HNode(l).rlink = i;
  HNode(r).llink = i;
  auto p = ULink(i);
  while (p != i) {
    UnHideC(p);
    p = ULink(p);
  }
}

void ExactCoverWithColorsSolver::UnHideC(int p) {
  auto q = p - 1;
  while (q != p) {
    const auto x = Top(q);
    const auto u = ULink(q);
    const auto d = DLink(q);
    const auto c = Color(q);
    if (x <= 0) {
      // q was a spacer
      q = d;
    } else if (c == CorrectColor) {
      q -= 1;
    } else {
      VNode(u).dlink = q;
      VNode(d).ulink = q;
      VNode(x).len_or_top += 1;
      q -= 1;
    }
  }
}

void ExactCoverWithColorsSolver::Commit(int p, int j) {
  if (Color(p) == 0) CoverC(j);
  if (Color(p) > 0) Purify(p);
}

void ExactCoverWithColorsSolver::Purify(int p) {
  auto c = Color(p);
  auto i = Top(p);
  auto q = DLink(i);
  while (q != i) {
    if (Color(q) == c) {
      VNode(q).color = CorrectColor;
    } else {
      HideC(q);
    }
    q = DLink(q);
  }
}

void ExactCoverWithColorsSolver::UnCommit(int p, int j) {
  if (Color(p) == 0) UnCoverC(j);
  if (Color(p) > 0) UnPurify(p);
}

void ExactCoverWithColorsSolver::UnPurify(int p) {
  auto c = Color(p);
  auto i = Top(p);
  auto q = ULink(i);
  while (q != i) {
    if (Color(q) == CorrectColor) {
      VNode(q).color = c;
    } else {
      UnHideC(q);
    }
    q = ULink(q);
  }
}
#pragma endregion
