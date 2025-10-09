#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>
#include "../models/InfoRuntime.hpp"
#include "../models/InfoProblem.hpp"

std::vector<RuntimeInfo> readLogger(const std::string& filePath, bool maximize = true, std::vector<Domain> domains = std::vector<Domain>()) {
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
                data.push_back(RuntimeInfo(maximize, domains));
            else
                data.push_back(RuntimeInfo(maximize));
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

InfoProblem readFolder(const std::string& folderPath) {    
    InfoProblem problem;
    string nameProblem = folderPath.substr(folderPath.find_last_of("_") + 1);
    problem.setName(nameProblem);
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        std::string path = entry.path();
        unsigned dimension = std::stoi(path.substr(path.find_last_of("_") + 4, path.find_last_of(".") - path.find_last_of("_") - 4));
        std::vector<RuntimeInfo> data = readLogger(path);
        problem.addRun(dimension, data);
    }

    return problem;
}

std::vector<InfoProblem> readSuite(const std::string& folderPath) {
    std::vector<InfoProblem> problems;
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        if (entry.is_directory()) {
            std::string path = entry.path();
            InfoProblem problem = readFolder(path);
            problems.push_back(problem);
        }
    }

    return problems;
}

void removeLogs(const std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        std::string path = entry.path();
        std::remove(path.c_str());
    }
}