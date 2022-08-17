#include <gtest/gtest.h>

#include "solver/dancing_links.h"

int runSolver(ExactCoverProblemSolver& solver, bool run_multi_thread) {
  if (run_multi_thread) {
    solver.SolveMultiThread(false);
  } else {
    solver.Solve(false);
  }
  const auto num_solutions = solver.NumSolutions();
  return num_solutions;
}

TEST(XC, Langford) {
  const auto solution_list =
      std::vector<int>{1, 1, 0, 0, 26, 150, 0, 0, 17792, 108144};
  for (auto run_multi_thread = 0; run_multi_thread <= 1; ++run_multi_thread) {
    for (auto size = 3; size <= 12; ++size) {
      const int num_items = 2 * size + size;
      auto option_list = std::vector<std::vector<int>>{};
      for (auto i = 1; i <= size; ++i) {
        for (auto j = 0; j < 2 * size; ++j) {
          const auto l = j;
          const auto r = j + i + 1;
          if (r < 2 * size) {
            option_list.emplace_back(std::vector<int>{l, r, 2 * size + i - 1});
          }
        }
      }

      auto solver = ExactCoverProblemSolver(num_items, option_list);
      const auto num_solutions = runSolver(solver, run_multi_thread == 1);

      EXPECT_EQ(solution_list[size - 3], num_solutions / 2)
          << "Failed size: " << size;
    }
  }
}
TEST(XC, NQueen) {
  const auto solution_list =
      std::vector<int>{1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200};
  for (auto run_multi_thread = 0; run_multi_thread <= 1; ++run_multi_thread) {
    for (auto size = 1; size <= 12; ++size) {
      const auto num_items = size + size + (2 * size - 1) + (2 * size - 1);
      const auto offset0 = 0;
      const auto offset1 = size;
      const auto offset2 = size + size;
      const auto offset3 = size + size + (2 * size - 1);
      auto option_list = std::vector<std::vector<int>>();

      for (auto x = 0; x < size; ++x) {
        for (auto y = 0; y < size; ++y) {
          auto option = std::vector<int>();
          option.push_back(offset0 + x);
          option.push_back(offset1 + y);
          option.push_back(offset2 + x + y);
          option.push_back(offset3 + x - y + size - 1);
          option_list.push_back(option);
        }
      }
      for (auto i = offset2; i < num_items; ++i) {
        option_list.push_back({i});
      }

      auto solver = ExactCoverProblemSolver(num_items, option_list);
      const auto num_solutions = runSolver(solver, run_multi_thread == 1);

      EXPECT_EQ(solution_list[size - 1], num_solutions)
          << "Failed size: " << size;
    }
  }
}
