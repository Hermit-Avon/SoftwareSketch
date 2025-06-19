#include <iostream>
#include <chrono>
#include <x86intrin.h> // 包含 __rdtsc()

void dummy_function() {
    volatile int sum = 0;
    for (int i = 0; i < 1000000; ++i) {
        sum += i;
    }
}

int main() {
    // 使用 chrono 记录开始时间
    auto start_time = std::chrono::high_resolution_clock::now();
    unsigned long long start_cycle = __rdtsc();

    // 要测试的代码
    dummy_function();

    // 记录结束时间和周期
    unsigned long long end_cycle = __rdtsc();
    auto end_time = std::chrono::high_resolution_clock::now();

    // 计算时间差（以纳秒为单位）
    auto elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    unsigned long long cycles = end_cycle - start_cycle;
    double time_in_ns = static_cast<double>(elapsed_ns.count());

    std::cout << "Elapsed Time (ns): " << time_in_ns << std::endl;
    std::cout << "CPU Cycles: " << cycles << std::endl;

    // 计算每纳秒的周期数（估计频率）
    double estimated_freq = cycles / time_in_ns * 1e9; // cycles per second
    std::cout << "Estimated CPU Frequency: " << estimated_freq / 1e9 << " GHz" << std::endl;

    return 0;
}
