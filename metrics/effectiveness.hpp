#include <vector>
#include <cmath>
#pragma once

// Feasible rate
double feasible_rate(const int f_runs, const int total_runs) {
    return f_runs / total_runs;
}

// Average number of function evaluation for optimality
double avg_feval_opt(const std::vector<int> fevals) {
    double sum = 0;
    for (int i = 0; i < fevals.size(); i++) {
        sum += fevals[i];
    }
    return sum / fevals.size();
}

// Success rate
double success_rate(const int s_runs, const int total_runs) {
    return s_runs / total_runs;
}

// Success performance
double success_perf(const std::vector<int> fevals, const int s_runs, const int total_runs) {
    return avg_feval_opt(fevals) / success_rate(s_runs, total_runs);
}

// Performance ratio
double perf_profile(const std::vector<int> times, const double t_s) {
    return t_s / *std::min_element(times.begin(), times.end());
}

// Performance profile of solver
double perf_profile_solver(const std::vector<int> times, const double tau) {
    int count = 0;
    for (int i = 0; i < times.size(); i++) {
        if (perf_profile(times, times[i]) <= tau) {
            count++;
        }
    }
    return count / times.size();
}

// Data profile of solver
double data_profile(const std::vector<int> times, const int n_p, const double k) {
    int count = 0;
    for (int i = 0; i < times.size(); i++) {
        if ((times[i] / (n_p + 1)) <= k) {
            count++;
        }
    }
    return count / times.size();
}