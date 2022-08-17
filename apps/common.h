#pragma once

#include <chrono>
#include <iomanip>

#include "solver/dancing_links.h"

inline int runSolver(const std::string& name, ExactCoverProblemSolver& solver,
                     bool save_solution) {
  const auto start = std::chrono::high_resolution_clock::now();
  solver.SolveMultiThread(save_solution);
  const auto end = std::chrono::high_resolution_clock::now();

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;

  const auto elapsed_sec =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "[Elapsed sec] ";
  std::cout << std::setw(15) << name;
  std::cout << ": " << elapsed_sec.count() << std::endl;
  return num_solutions;
}
