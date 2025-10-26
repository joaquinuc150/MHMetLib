#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
using namespace std;

template <typename T>
void print_vector(string var, std::vector<T> v, std::ofstream* file = nullptr, size_t n = -1) {
    n = (n == -1 || n > v.size()) ? v.size() : n;
    if(file != nullptr){
        (*file) << var << ": ";
        for (int i = 0; i < n; i++) {
            (*file) << v[i] << " ";
        }
        (*file) << std::endl;
        return;
    }
    std::cout << var << ": ";
    for (int i = 0; i < n; i++) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl;
}

void output_file_vector(std::string filePath, std::string fileName, std::vector<double> v) {
    std::filesystem::create_directories(filePath); 
    std::ofstream file(filePath + fileName + ".txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath + fileName + ".txt" << std::endl;
        return;
    }
    for (int i = 0; i < v.size(); i++) {
        file << v[i] << " ";
    }
    file.close();
}

void output_file_matrix(std::string filePath, std::string fileName, std::vector<std::vector<double>> m) {
    std::filesystem::create_directories(filePath); 
    std::ofstream file(filePath + fileName + ".txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath + fileName + ".txt" << std::endl;
        return;
    }
    for (int i = 0; i < m.size(); i++) {
        for (int j = 0; j < m[i].size(); j++) {
            file << m[i][j] << " ";
        }
        file << std::endl;
    }
    file.close();
}

void print_double(string var, double v, std::ofstream* file = nullptr) {
    if(file != nullptr){
        (*file) << var << ": " << v << std::endl;
        return;
    }
    std::cout << var << ": " << v << std::endl;
}