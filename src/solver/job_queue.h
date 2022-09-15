#pragma once

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

template <typename InputType, typename OutputType, typename Function>
class JobQueue {
 public:
  JobQueue(int i_num_threads, Function i_function)
      : num_threads(i_num_threads), function(i_function) {
    std::cout << "Num threads: " << num_threads << std::endl;
  }
  void AddJob(InputType input) {
    const auto lock = std::unique_lock<std::mutex>(input_mtx_);
    num_jobs_++;
    input_queue_.push(input);
  }
  void Start() {
    for (auto i = 0; i < num_threads; ++i) {
      thread_list_.emplace_back(std::thread(
          [](std::mutex* input_mtx, std::queue<InputType>* input_queue,
             std::mutex* output_mtx, std::queue<OutputType>* output_queue,
             Function function, std::atomic_bool* finish) {
            while (true) {
              if (*finish) break;

              // Get input mtx
              auto input_lock =
                  std::unique_lock<std::mutex>(*input_mtx, std::try_to_lock);
              if (not input_lock.owns_lock()) continue;

              // Get input
              if (input_queue->empty()) continue;
              const auto input = input_queue->front();
              input_queue->pop();
              input_lock.unlock();

              // Main process
              const auto output = function(input);

              // Get output mtx
              const auto output_lock =
                  std::unique_lock<std::mutex>(*output_mtx);
              output_queue->push(output);
            }

            return 0;
          },
          &input_mtx_, &input_queue_, &output_mtx_, &output_queue_, function,
          &finish_));
    }
  }
  void Join() {
    while (true) {
      const auto output_lock = std::unique_lock<std::mutex>(output_mtx_);
      if ((int)output_queue_.size() == num_jobs_) {
        finish_ = true;
        break;
      }
    }
    for (auto&& thread : thread_list_) thread.join();
  }

 private:
  const int num_threads;
  const Function function;

  int num_jobs_ = 0;
  std::atomic_bool finish_ = std::atomic_bool{false};
  std::vector<std::thread> thread_list_;
  std::mutex input_mtx_;
  std::queue<InputType> input_queue_;
  std::mutex output_mtx_;
  std::queue<OutputType> output_queue_;
};
