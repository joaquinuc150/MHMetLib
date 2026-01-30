#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include "../metrics/TrajMetrics.hpp"

std::vector<RuntimeInfo> readLogger(const std::string& filePath, bool maximize = true, std::vector<Domain_T> domains = std::vector<Domain_T>(), double threshold = 0.0, bool hasOptimum = false, double optimum = 0.0) {
    std::vector<RuntimeInfo> data;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return data;
    }

    std::string line;
    std::string header;
    std::getline(file, header); // Skip the first line
    int i = -1;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if(line == header || i == -1) {
            if (domains.size() > 0)
                data.push_back(RuntimeInfo(maximize, domains, threshold, hasOptimum, optimum));
            else
                data.push_back(RuntimeInfo(maximize, threshold, hasOptimum, optimum));
            i++;
        }
        if(line != header){
            std::istringstream iss(line);
            int iteration;
            double fitness;
            iss >> iteration >> fitness;

            data[i].addFevals(fitness);

            std::vector<double> positions;
            double position;
            while (iss >> position) {
                positions.push_back(position);
            }
            data[i].addX(positions);
        }

    }

    file.close();

    return data;
}

void removeLogs(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        std::string path = entry.path();
        std::remove(path.c_str());
    }
}