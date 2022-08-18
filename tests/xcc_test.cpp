#include <gtest/gtest.h>

#include "solver/dancing_links.h"

int runXCCSolver(ExactCoverWithColorsSolver& solver, bool run_multi_thread) {
  if (run_multi_thread) {
    solver.SolveMultiThread(false);
  } else {
    solver.Solve(false);
  }
  const auto num_solutions = solver.NumSolutions();
  return num_solutions;
}

TEST(XCC, NQueen) {
  const auto solution_list =
      std::vector<int>{1, 0, 0, 2, 10, 4, 40, 92, 352, 724, 2680, 14200};
  for (auto run_multi_thread = 0; run_multi_thread <= 1; ++run_multi_thread) {
    for (auto size = 1; size <= 12; ++size) {
      const auto num_primary_items = size + size;
      const auto num_secondary_items = (2 * size - 1) + (2 * size - 1);
      const auto offset0 = 0;
      const auto offset1 = size;
      const auto offset2 = size + size;
      const auto offset3 = size + size + (2 * size - 1);
      auto option_list = std::vector<std::vector<std::pair<int, int>>>();

      for (auto x = 0; x < size; ++x) {
        for (auto y = 0; y < size; ++y) {
          auto option = std::vector<std::pair<int, int>>();
          option.push_back({offset0 + x, 0});
          option.push_back({offset1 + y, 0});
          option.push_back({offset2 + x + y, 0});
          option.push_back({offset3 + x - y + size - 1, 0});
          option_list.push_back(option);
        }
      }

      auto solver = ExactCoverWithColorsSolver(
          num_primary_items, num_secondary_items, option_list);
      const auto num_solutions = runXCCSolver(solver, run_multi_thread == 1);

      EXPECT_EQ(solution_list[size - 1], num_solutions)
          << "Failed size: " << size;
    }
  }
}
