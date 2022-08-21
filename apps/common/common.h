#pragma once

#include <chrono>
#include <iomanip>

#include "solver/dancing_links.h"
#include "solver/option_handler.h"

inline int runXCSolver(const std::string& name, ExactCoverProblemSolver& solver,
                       bool save_solution) {
  const auto start = std::chrono::high_resolution_clock::now();
  solver.SolveMultiThread(save_solution);
  const auto end = std::chrono::high_resolution_clock::now();

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;

  const auto elapsed_sec =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "[Elapsed sec] ";
  std::cout << std::setw(30) << "XC " + name;
  std::cout << ": " << elapsed_sec.count() << std::endl;
  return num_solutions;
}

inline int runXCCSolver(const std::string& name,
                        ExactCoverWithColorsSolver& solver,
                        bool save_solution) {
  const auto start = std::chrono::high_resolution_clock::now();
  solver.SolveMultiThread(save_solution);
  const auto end = std::chrono::high_resolution_clock::now();

  const auto num_solutions = solver.NumSolutions();
  std::cout << "Num solutions: " << num_solutions << std::endl;

  const auto elapsed_sec =
      std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  std::cout << "[Elapsed sec] ";
  std::cout << std::setw(30) << "XCC " + name;
  std::cout << ": " << elapsed_sec.count() << std::endl;
  return num_solutions;
}
