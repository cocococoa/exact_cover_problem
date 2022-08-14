#pragma once

#include <chrono>

#include "solver/dancing_links.h"

inline int runSolver(ExactCoverProblemSolver& solver, bool save_solution) {
  const auto start = std::chrono::high_resolution_clock::now();

  std::cout << "Find exact cover via dancing links" << std::endl;
  solver.SolveMultiThread(save_solution);
  std::cout << "Done" << std::endl;

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;

  const auto end = std::chrono::high_resolution_clock::now();
  const auto elapsed_sec =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "Elapsed: " << elapsed_sec.count() << " [seconds]" << std::endl;
  return num_solutions;
}
