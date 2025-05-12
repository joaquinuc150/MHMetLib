#pragma once

double difference_absolute(double a, double b) {
    return (a-b) > 0 ? a-b : b-a; 
}

double multiply(double a, double b) {
    return a * b;
}

int plus_one(int a) {
    return a++;
}

long long nCr(int n, int r) {
    if (r > n) return 0;
    if (r == 0 || r == n) return 1;

    if (r > n - r) r = n - r;

    long long result = 1;
    for (int i = 0; i < r; i++) {
        result *= n - i;
        result /= i + 1;
    }

    return result;
}