#pragma once

#include <vector>
#include "./InfoRuntime.hpp"
#include "../metrics/effectiveness.hpp"

class InfoProblem {
    private:
        std::string name;
        std::vector<unsigned> dimensions;
        std::vector<std::vector<RuntimeInfo>> runs;
        std::vector<double> optimals;
        bool maximization = true;
    public:
        InfoProblem() {
            name = "";
            runs = {};
            dimensions = {};
            optimals = {};
        }

        InfoProblem(std::string name, std::vector<unsigned> dimensions, std::vector<std::vector<RuntimeInfo>> runs, bool maximization = true)
            : name(name), dimensions(dimensions), runs(runs), maximization(maximization) {}

        InfoProblem(std::string name, std::vector<unsigned> dimensions, std::vector<std::vector<RuntimeInfo>> runs, std::vector<double> optimals, bool maximization = true)
            : name(name), dimensions(dimensions), runs(runs), optimals(optimals), maximization(maximization) {}

        void setName(std::string name) {
            name = name;
        }

        void addOptimal(double optimal) {
            optimals.push_back(optimal);
        }

        void addRun(unsigned dimension, std::vector<RuntimeInfo> run) {
            dimensions.push_back(dimension);
            runs.push_back(run);
        }

        void setOptimal(int i) {
            std::vector<double> bests;
            for (int j = 0; j < runs[i].size(); j++) {
                bests.push_back(runs[i][j].getfBest());
            }
            optimals.push_back(*std::max_element(bests.begin(), bests.end()));
        }

        double getAvgFeval(int i) {
            std::vector<int> fevals;
            for (int j = 0; j < runs[i].size(); j++) {
                fevals.push_back(runs[i][j].getFevalsSize());
            }
            return avg_feval_opt(fevals);
        }
        
};