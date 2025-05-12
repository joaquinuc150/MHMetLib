#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>
#include "../utils/math.hpp"

// Convergence rate is |f_i+1 - f_i| / |f_i - f_i-1|
double conv_rate_absolute(const double f_iplusone, const double f_i, const double f_iminusone) {
    if (f_i == f_iminusone)
        return 0;
    return difference_absolute(f_iplusone, f_i) / difference_absolute(f_i, f_iminusone);
}

// Convergence rate with signed value
double conv_rate(const double f_iplusone, const double f_i, const double f_iminusone, const bool maximize = true) {
    if (f_i == f_iminusone)
        return 0;
    return maximize ? 
        (f_iplusone - f_i) / (f_i - f_iminusone) :
        (f_i - f_iplusone) / (f_iminusone - f_i);
}

// Average convergance rate of solution
double avg_conv_rate(const double f_average, const double f_best) {
    return(1 - difference_absolute(f_average, f_best) / f_best);
}

// Convergence rate with opt value
double conv_rate_opt(const double f_i, const double f_opt, const double f_iminusone) {
    if (f_opt == f_iminusone)
        return 0;
    return difference_absolute(f_i, f_opt) / difference_absolute(f_iminusone, f_opt);
}

// Error rate is 1 - f_i / f_best
double error_rate(const double f_i, const double f_best, const bool maximize = true) {
    return maximize ?
        (1 - f_i / f_best):
        (1 - f_best / f_i);
}

// Error rate without best value
double error_rate_without_best(const double f_i, const double f_iplusone) {
    return difference_absolute(f_i, f_iplusone) / abs(f_i);
}

// Geometric rate of fitness is 1 - (|f_opt - f_i| / |f_opt - f_0|)^(1 / i)
double geom_rate(const double f_opt, const double f_i, const double f_0, const int i) {
    return 1 - pow(difference_absolute(f_opt, f_i) / difference_absolute(f_opt, f_0), (double) 1 / i);
}

// Average of dimension j of n solutions of vector x
double avg_dim(std::vector<std::vector<double>> x, const int j) {
    double sum = 0;
    int n = x.size();
    for (int i = 0; i < n; i++) {
        sum += x[i][j];
    }
    return sum / n;
}

// Divesity of dimension j in n solutions of vector x
double diversity_dim(std::vector<std::vector<double>> x, const int j) {
    int n = x.size();
    double avg = avg_dim(x, j);
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += difference_absolute(x[i][j], avg);
    }
    return sum / n;
}

// Diversity of all dimension of solutions with weight w, default w = 1
double diversity(std::vector<std::vector<double>> x, const double w = 1.0) {
    double sum = 0;
    int n = x[0].size();
    for (int j = 0; j < n; j++) {
        sum += w * diversity_dim(x, j);
    }
    return sum / n;
}

double diversity_dim_optimized(std::vector<std::vector<double>> x, std::vector<double> sum_dim) {
    int n = x.size();
    for (int i = 0; i < sum_dim.size(); i++)
    {
        sum_dim[i] /= n;
    }
    double sum = 0;
    for (int i = 0; i < n; i++) {
        std::transform(x[i].begin(), x[i].end(), sum_dim.begin(), x[i].begin(), difference_absolute);
        sum += std::reduce(x[i].begin(), x[i].end(), 0.0);
    }

    return sum / n;
}

double diversity_optimized(std::vector<std::vector<double>> x, std::vector<double> sum_dim, const double w = 1.0) {
    double sum = 0;
    int n = x[0].size();
    sum = diversity_dim_optimized(x, sum_dim);
    return sum / n;
}

// Distance hamming between two solutions
double distance_hamming(std::vector<double> x1, std::vector<double> x2) {
    int n = x1.size();
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += x1[i] != x2[i];
    }
    return sum;
}

// Hamming distance between two solutions, normalized by the number of dimensions
double distance_hamming_normalized(std::vector<double> x1, std::vector<double> x2) {
    return distance_hamming(x1, x2) / x1.size();
}

// Time ratio between converge to 5% of best solution and converge to best solution
double time_ratio(const int t_fivepercentage, const int t_best) {
    return t_fivepercentage / t_best;
}

// Ratio between unsucccessful runs + successful runs in susccessful runs
double ratio_unsuc_suc(const int run_unsuc, const int run_suc) {
    return (run_unsuc + run_suc) / run_suc;
}

double ecdf(std::vector<double> t, const double t_i) {
    int count = 0;
    int n = t.size();
    for (int i = 0; i < n; i++) {
        if (t[i] <= t_i) {
            count++;
        }
    }
    return count / n;
}

// Page L statistic, Method construct order from k treatments on N samples and ranked from the best (with 1) to the worst (with k)
double page_l_statistic(std::vector<double> t, const double k) {
    std::sort(t.begin(), t.end());
    double sum = 0;
    for (int i = 0; i < k; i++) {
        sum += t[i] * (i + 1);
    }
    return sum;
}