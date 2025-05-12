#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <map>
#include <eigen3/Eigen/Dense>
#include "SimpleMatrix.h"
#include <unordered_set>
#include <cfloat>
namespace ioh {
    namespace common {
        class MetricsAnalyzer {
            private:
                int n_var, n_firstGen, n_children; //Variables for variable number, first generation solutions and children number
                bool maxProblem = false; //Boolean to determine if the problem is maximization or minimization
                std::vector<std::vector<double>> data;
                std::vector<std::string> dataNames;
                std::vector<std::vector<double>> xVectorsPerEvaluation;
                std::vector<std::vector<std::vector<double>>> xVectorsPerGeneration;
                std::vector<std::vector<std::vector<double>>> xVectorsProcessed;
                std::vector<std::vector<double>> hammingDistances;
                std::vector<std::vector<int>> searchSpaceAreasBySolutions; //
                std::vector<std::vector<std::vector<int>>> searchSpaceAreasBySearchAgent; //Vector de agentes, cada agente tiene un vector por sector de espacio de busqueda
                std::vector<std::vector<int>> searchSpaceAreasByGenerations; //Cada generacion tiene un vector donde la posicion es el agente y el valor es el sector de espacio de busqueda
                Eigen::MatrixXd dissimilarity_matrix;
                Eigen::MatrixXd xMatrix;
                std::vector<std::vector<double>> objectiveValuePerGeneration;
                std::vector<std::vector<double>> crossoverImprovement;
                std::vector<std::vector<double>> mutationImprovement;

            public:
            //SECCION 1: UTILIDADES
                //Initialize n_var, n_firstGen and n_children
                MetricsAnalyzer(int n_var, int n_firstGen, int n_children, bool maxProblem) : n_var(n_var), n_firstGen(n_firstGen), n_children(n_children), maxProblem(maxProblem) {}
                std::vector<std::string> splitString(const std::string& str, char delimiter) {
                    std::vector<std::string> tokens;
                    std::istringstream iss(str);
                    std::string token;

                    while (std::getline(iss, token, delimiter)) {
                        tokens.push_back(token);
                    }

                    return tokens;
                }

                long long nCr(int n, int r) {
                    if (r > n) return 0;
                    if (r == 0 || r == n) return 1;

                    // Ensure r is the smaller of n-r and r for efficiency
                    if (r > n - r) r = n - r;

                    long long result = 1;
                    for (int i = 0; i < r; ++i) {
                        result *= (n - i);
                        result /= (i + 1);
                    }
                    return result;
                }

                int getIndex(std::vector<std::string> v, std::string K) { 
                    auto it = std::find(v.begin(), v.end(), K); 
                    if (it != v.end())  
                    { 
                        int index = it - v.begin(); 
                        return index;
                    } 
                    else { 
                        return -1;
                    }
                } 
                void readDataFromFile(const std::string& filename) {
                    std::ifstream file(filename);
                    if (file.is_open()) {
                        std::string name;

                        std::getline(file, name);
                        std::istringstream iss(name);
                        std::string dataName;
                        while (std::getline(iss, dataName, ' ')) {
                            dataNames.push_back(dataName);
                            data.push_back(std::vector<double>());
                        }

                        double value;
                        int i = 0;
                        while (file >> value) {
                            data[i % dataNames.size()].push_back(value);
                            ++i;
                        }

                        file.close();

                    } else {
                        std::cerr << "Failed to open data file: " << filename << std::endl;
                    }

                    /*
                    std::cout << "Data names: ";
                    for (const std::string& name : dataNames) {
                        std::cout << name << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "Data: " << std::endl;
                    for (size_t i = 0; i < data.size(); ++i) {
                        std::cout << dataNames[i] << ": ";
                        for (size_t j = 0; j < data[i].size(); ++j) {
                            std::cout << data[i][j] << " ";
                        }
                        std::cout << std::endl;
                    }*/
                }
                //No longer needed
                void postProcessData(int n_firstGen, int n_children){
                    int childrenCounter = 0;
                    int genCounter = 1;
                    int genIndex = getIndex(dataNames, "generation");
                    for (size_t i = n_firstGen; i < data[genIndex].size(); ++i) {
                        if (data[genIndex][i] == genCounter) {
                            ++childrenCounter;
                            if (childrenCounter == n_children) {
                                ++genCounter;
                                childrenCounter = 0;
                            }
                        }
                        if (genCounter < data[genIndex][i]) {
                            for (int j = 0; j < n_children - childrenCounter; ++j) {
                                for (size_t k = 0; k < dataNames.size(); ++k) {
                                    data[k].insert(data[k].begin() + i, data[k][i - 1]);
                                }
                            }
                            childrenCounter = 0;
                            ++genCounter;
                        }
                    }
                    childrenCounter = 0;
                    genCounter = 1;
                    bool flag = false;
                    for (size_t i = n_firstGen; i < data[genIndex].size(); ++i){
                        ++childrenCounter;
                        if (data[genIndex][i] == genCounter && flag == false) { 
                            if (childrenCounter == n_children && data[genIndex][i + 1] == genCounter + 1) {
                                ++genCounter;
                                childrenCounter = 0;
                            }
                        }
                        if (childrenCounter > n_children && data[genIndex][i + 1] != genCounter + 1) {
                            flag = true;
                        }
                        if (flag == true && data[genIndex][i + 1] > genCounter + 1) {
                            int counterTemp = 0;
                            for (int j = 0; j < childrenCounter - n_children; ++j) {
                                data[genIndex][i - childrenCounter + j + n_children + 1] = genCounter + 1;
                                counterTemp++;
                                if (counterTemp == n_children) {
                                    counterTemp = 0;
                                    ++genCounter;
                                }
                            }
                            childrenCounter = 0;
                            ++genCounter;
                            flag = false;
                        }
                    }
                    //Case for last generation
                    if (data[genIndex][data[genIndex].size() - 1] != data[genIndex][data[genIndex].size() - 2]){
                        for (int j = 0; j < n_children - 1; ++j) {
                            for (size_t k = 0; k < dataNames.size(); ++k) {
                                data[k].push_back(data[k][data[k].size() - 1]);
                            }
                        }
                    }
                }

                void createXVectorsPerEvaluation(int n_var){
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return;
                    }
                    
                    int index = getIndex(dataNames, "x0");
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        xVectorsPerEvaluation.push_back(std::vector<double>());
                        xVectorsPerEvaluation[i].push_back(data[index][i]);
                    }
                    for (int i = 1; i < n_var; ++i) {
                        index = getIndex(dataNames, "x" + std::to_string(i));
                        for (size_t j = 0; j < data[index].size(); ++j) {
                            xVectorsPerEvaluation[j].push_back(data[index][j]);
                        }
                    }
                }

                void createXMatrixPerEvaluation(){
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return;
                    }
                    int index = getIndex(dataNames, "x0");
                    Eigen::MatrixXd xMatrix(data[index].size(), dataNames.size() - index);
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        xMatrix(i, 0) = data[index][i];
                    }
                    for (size_t i = 1; i < dataNames.size() - 1; ++i) {
                        index = getIndex(dataNames, "x" + std::to_string(i));
                        for (size_t j = 0; j < data[index].size(); ++j) {
                            xMatrix(j, i) = data[index][j];
                        }
                    }
                    this->xMatrix = xMatrix;
                    //std::cout << xMatrix << std::endl;
                }

                void createXVectorsPerGeneration(){
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return;
                    }
                    int genIndex = getIndex(dataNames, "generation");
                    double numberOfGenerations = data[genIndex][data[genIndex].size() - 1];
                    int index = getIndex(dataNames, "x0");
                    for (size_t i = 0; i < numberOfGenerations + 1; ++i) {
                        xVectorsPerGeneration.push_back(std::vector<std::vector<double>>());
                        if (i == 0){
                            for (size_t j = 0; j < n_firstGen; ++j) {
                                xVectorsPerGeneration[i].push_back(std::vector<double>());
                            }
                        } else {
                            for (size_t j = 0; j < n_children; ++j) {
                                xVectorsPerGeneration[i].push_back(std::vector<double>());
                            }
                        }
                    }
                    
                    for (int i = 0; i < n_var; ++i) {
                        index = getIndex(dataNames, "x" + std::to_string(i));
                        int countPerGen = 0;
                        for (size_t j = 0; j < data[index].size(); ++j) {
                            if (data[genIndex][j] == 0) {
                                xVectorsPerGeneration[0][countPerGen].push_back(data[index][j]);
                                ++countPerGen;
                                if (countPerGen == n_firstGen) {
                                    countPerGen = 0;
                                }
                            } else {
                                xVectorsPerGeneration[data[genIndex][j]][countPerGen].push_back(data[index][j]);

                                ++countPerGen;
                                if (countPerGen == n_children) {
                                    countPerGen = 0;
                                }
                            }
                        }
                    }
                }

                void createObjectiveValuePerGeneration(){
                    if(objectiveValuePerGeneration.size() > 0){
                        return;
                    }
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return;
                    }
                    int genIndex = getIndex(dataNames, "generation");
                    int index = getIndex(dataNames, "raw_y");
                    int counter = 0;
                    //FirstGen
                    objectiveValuePerGeneration.push_back(std::vector<double>());
                    for (int i = 0; i < n_firstGen; ++i) {
                        objectiveValuePerGeneration[0].push_back(0);
                    }
                    //Rest of generations
                    for (size_t i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        objectiveValuePerGeneration.push_back(std::vector<double>());
                        for (int j = 0; j < n_children; ++j) {
                            objectiveValuePerGeneration[i].push_back(0);
                        }
                    }
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            objectiveValuePerGeneration[0][i] = data[index][i];
                        } else {
                            if (data[genIndex][i] != data[genIndex][i - 1]) {
                                counter = 0;
                            }
                            objectiveValuePerGeneration[data[genIndex][i]][counter] = data[index][i];
                            ++counter;
                        }
                    }
                    //PostProcessing
                    for (size_t i = 0; i < objectiveValuePerGeneration.size(); ++i) {
                        for (size_t j = 0; j < objectiveValuePerGeneration[i].size(); ++j) {
                            if (objectiveValuePerGeneration[i][j] == 0) {
                                objectiveValuePerGeneration[i][j] = objectiveValuePerGeneration[i - 1][j];
                            }
                        }
                    }
                    /*
                    for (size_t i = 0; i < objectiveValuePerGeneration.size(); ++i) {
                        for (size_t j = 0; j < objectiveValuePerGeneration[i].size(); ++j) {
                            std::cout << "Generation " << i << " Agent " << j << " Objective Value: " << objectiveValuePerGeneration[i][j] << std::endl;
                        }
                    }*/
                }

                void postProcessGenerations(){
                    std::vector<std::vector<std::vector<double>>> xVectorsProcessed;
                    xVectorsProcessed.reserve(xVectorsPerGeneration.size());

                    for (size_t i = 0; i < xVectorsPerGeneration.size(); ++i) {
                        if (xVectorsPerGeneration[i].empty()) {
                            continue;
                        }
                        
                        xVectorsProcessed.push_back(std::vector<std::vector<double>>());
                        xVectorsProcessed.back().reserve(std::max(static_cast<size_t>(n_children), xVectorsPerGeneration[i].size()));
                        
                        for (size_t j = 0; j < xVectorsPerGeneration[i].size(); ++j) {
                            if (i > 0 && xVectorsPerGeneration[i][j].empty() && j < xVectorsPerGeneration[i-1].size()) {
                                xVectorsProcessed.back().push_back(xVectorsProcessed[i-1][j]);
                            } else if (!xVectorsPerGeneration[i][j].empty()) {
                                xVectorsProcessed.back().push_back(xVectorsPerGeneration[i][j]);
                            }
                        }
                        
                        if (i > 0 && xVectorsProcessed.back().size() < static_cast<size_t>(n_children)) {
                            size_t current_size = xVectorsProcessed.back().size();
                            for (size_t j = current_size; j < static_cast<size_t>(n_children); ++j) {
                                size_t prev_index = j % xVectorsProcessed[i-1].size();
                                xVectorsProcessed.back().push_back(xVectorsProcessed[i-1][prev_index]);
                            }
                        }
                    }
                    this->xVectorsProcessed = xVectorsProcessed;
                }

                double bitFlipDistance(std::vector<double> x1, std::vector<double> x2){
                    if (x1.size() != x2.size()) {
                        std::cerr << "Vectors must have the same length" << std::endl;
                        return -1;
                    }
                    int distance = 0;
                    for (size_t i = 0; i < x1.size(); ++i) {
                        if (x1[i] != x2[i]) {
                            ++distance;
                        }
                    }
                    return distance;
                }

                //Create specifier for type of operator (swap, bit-flip, etc)
                double hammingDistance(std::vector<double> x1, std::vector<double> x2){
                    if (x1.size() != x2.size()) {
                        std::cerr << "Vectors must have the same length" << std::endl;
                        return -1;
                    }
                    double res = bitFlipDistance(x1, x2); 
                    return res;
                }

                std::vector<std::vector<double>> createhammingDistanceMatrix(){
                    if (xVectorsPerEvaluation.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return std::vector<std::vector<double>>();
                    }
                    for (size_t i = 0; i < xVectorsPerEvaluation.size(); ++i) {
                        hammingDistances.push_back(std::vector<double>());
                        for (size_t j = 0; j < xVectorsPerEvaluation.size(); ++j) {
                            hammingDistances[i].push_back(0);
                        }
                    }
                    for (size_t i = 0; i < xVectorsPerEvaluation.size(); ++i) {
                        for (size_t j = 0; j < xVectorsPerEvaluation.size(); ++j) {
                            hammingDistances[i][j] = hammingDistance(xVectorsPerEvaluation[i], xVectorsPerEvaluation[j]);
                            hammingDistances[j][i] = hammingDistances[i][j];
                        }
                    }
                    return hammingDistances;
                }

                void createDissimilarityMatrix(){
                    Eigen::MatrixXd dissimilarityMatrix(xVectorsPerEvaluation.size(), xVectorsPerEvaluation.size());
                    for (size_t i = 0; i < xVectorsPerEvaluation.size(); ++i) {
                        for (size_t j = 0; j < xVectorsPerEvaluation.size(); ++j) {
                            dissimilarityMatrix(i, j) = hammingDistance(xVectorsPerEvaluation[i], xVectorsPerEvaluation[j]);
                        }
                    }
                    dissimilarity_matrix = dissimilarityMatrix;
                    //std::cout << "Dissimilarity Matrix: " << std::endl;
                    //std::cout << dissimilarity_matrix << std::endl;
                }

                void MDS3(){
                    createXMatrixPerEvaluation();
                    createDissimilarityMatrix();
                    smat::Matrix<double> *X0 = new smat::Matrix<double>(xMatrix.rows(), xMatrix.cols(), 0.0);
                    for (size_t i = 0; i < xMatrix.rows(); ++i) {
                        for (size_t j = 0; j < xMatrix.cols(); ++j) {
                            X0->set(i, j, xMatrix(i, j));
                        }
                    }
                    smat::Matrix<double> *D = new smat::Matrix<double>(dissimilarity_matrix.rows(), dissimilarity_matrix.cols(), 0.0);
                    for (size_t i = 0; i < dissimilarity_matrix.rows(); ++i) {
                        for (size_t j = 0; j < dissimilarity_matrix.cols(); ++j) {
                            D->set(i, j, dissimilarity_matrix(i, j));
                        }
                    }
                    smat::Matrix<double> *X1 = MDS_SMACOF(D, NULL, 3, 30); 
                    X1->saveTxt("X1.txt");
                    //smat -> eigen?
                }

                void MDSSearchSpaceDivisionBySolutions(){
                    MDS3();
                    //Open X1.txt
                    std::vector <std::vector<int>> X1;
                    std::ifstream file("X1.txt");
                    int counter = 0;
                    int maxX = 0;
                    int minX = 0;
                    int maxY = 0;
                    int minY = 0;
                    int maxZ = 0;
                    int minZ = 0;
                    if (file.is_open()) {
                        std::string line;
                        while (std::getline(file, line)) {
                            std::vector<int> columns;
                            std::string value;
                            int valueCast;
                            double valueCast2;
                            std::stringstream ss(line);
                            while (ss >> value){
                                valueCast2 = std::stod(value);
                                valueCast = std::round(valueCast2);
                                if (counter == 0)
                                    if (valueCast > maxX)
                                        maxX = valueCast;
                                    if (valueCast < minX)
                                        minX = valueCast;
                                else if (counter == 1)
                                    if (valueCast > maxY)
                                        maxY = valueCast;
                                    if (valueCast < minY)
                                        minY = valueCast;
                                else if (counter == 2){
                                    if (valueCast > maxZ)
                                        maxZ = valueCast;
                                    if (valueCast < minZ)
                                        minZ = valueCast;
                                }
                                counter++;
                                if (counter == 3)
                                    counter = 0;
                                columns.push_back(valueCast);
                            }
                            X1.push_back(columns);
                        }
                        file.close();
                    } else {
                        std::cerr << "Failed to open data file: " << "X1.txt" << std::endl;
                    }
                    //SearchSpaceDivision
                    int midX = (maxX + minX) / 2;
                    int midY = (maxY + minY) / 2;
                    int midZ = (maxZ + minZ) / 2;
                    
                    for (size_t i = 0; i < 8; ++i) {
                        searchSpaceAreasBySolutions.push_back(std::vector<int>());
                    }
                    //std::cout << "X1 size: " << X1.size() << std::endl;
                    for (size_t i = 0; i < X1.size(); ++i) {
                        if (X1[i][0] < midX && X1[i][1] < midY && X1[i][2] < midZ) {
                            searchSpaceAreasBySolutions[0].push_back(i);
                        } else if (X1[i][0] < midX && X1[i][1] < midY && X1[i][2] >= midZ) {
                            searchSpaceAreasBySolutions[1].push_back(i);
                        } else if (X1[i][0] < midX && X1[i][1] >= midY && X1[i][2] < midZ) {
                            searchSpaceAreasBySolutions[2].push_back(i);
                        } else if (X1[i][0] < midX && X1[i][1] >= midY && X1[i][2] >= midZ) {
                            searchSpaceAreasBySolutions[3].push_back(i);
                        } else if (X1[i][0] >= midX && X1[i][1] < midY && X1[i][2] < midZ) {
                            searchSpaceAreasBySolutions[4].push_back(i);
                        } else if (X1[i][0] >= midX && X1[i][1] < midY && X1[i][2] >= midZ) {
                            searchSpaceAreasBySolutions[5].push_back(i);
                        } else if (X1[i][0] >= midX && X1[i][1] >= midY && X1[i][2] < midZ) {
                            searchSpaceAreasBySolutions[6].push_back(i);
                        } else if (X1[i][0] >= midX && X1[i][1] >= midY && X1[i][2] >= midZ) {
                            searchSpaceAreasBySolutions[7].push_back(i);
                        } else {
                            std::cout << "Error in search space division" << std::endl;
                        }
                    }   
                        /* 
                    std::cout << "Search Space Division by Solutions" << std::endl;
                    for (size_t i = 0; i < searchSpaceAreasBySolutions.size(); ++i) {
                        std::cout << "Area " << i << ": ";
                        std::cout << "Area size: " << searchSpaceAreasBySolutions[i].size() << " ";
                        std::cout << std::endl;
                        std::cout << "Area elements: ";
                        std::cout << std::endl;
                        for (size_t j = 0; j < searchSpaceAreasBySolutions[i].size(); ++j) {
                            std::cout << searchSpaceAreasBySolutions[i][j] << " ";
                        }
                        std::cout << std::endl;
                    }*/   
                }

                void MDSSearchSpaceDivisionCustomSectors(int targetSectors) {
                    // Calculate divisions needed per dimension to approximate target sectors
                    int divisionsPerDim = std::ceil(std::cbrt(targetSectors));
                    int actualSectors = std::pow(divisionsPerDim, 3);

                    std::cout << "Divisions per dimension: " << divisionsPerDim << std::endl;
                    std::cout << "Actual sectors: " << actualSectors << std::endl;
                    
                    MDS3();
                    std::vector<std::vector<int>> X1;
                    std::vector<double> maxVals = {-DBL_MAX, -DBL_MAX, -DBL_MAX};
                    std::vector<double> minVals = {DBL_MAX, DBL_MAX, DBL_MAX};
                    
                    // Read file and find bounds
                    std::ifstream file("X1.txt");
                    if (file.is_open()) {
                        std::string line;
                        while (std::getline(file, line)) {
                            std::vector<int> point;
                            std::stringstream ss(line);
                            std::string value;
                            int dim = 0;
                            while (ss >> value && dim < 3) {
                                double val = std::stod(value);
                                maxVals[dim] = std::max(maxVals[dim], val);
                                minVals[dim] = std::min(minVals[dim], val);
                                point.push_back(std::round(val));
                                dim++;
                            }
                            X1.push_back(point);
                        }
                        file.close();
                    } else {
                        std::cerr << "Failed to open X1.txt" << std::endl;
                        return;
                    }
                
                    std::vector<double> stepSizes;
                    for (int i = 0; i < 3; i++) {
                        stepSizes.push_back((maxVals[i] - minVals[i]) / divisionsPerDim);
                    }
                
                    searchSpaceAreasBySolutions.resize(actualSectors);
                
                    for (size_t i = 0; i < X1.size(); i++) {
                        std::vector<int> sectorIndices;
                        for (int dim = 0; dim < 3; dim++) {
                            double val = X1[i][dim];
                            int index = std::min(static_cast<int>((val - minVals[dim]) / stepSizes[dim]), 
                                               divisionsPerDim - 1);
                            sectorIndices.push_back(index);
                        }
                        
                        int sectorIndex = sectorIndices[0] * std::pow(divisionsPerDim, 2) +
                                         sectorIndices[1] * divisionsPerDim +
                                         sectorIndices[2];
                        
                        searchSpaceAreasBySolutions[sectorIndex].push_back(i);
                    }
                    
                    // If we have more sectors than requested, merge excess sectors
                    if (actualSectors > targetSectors) {
                        std::vector<std::vector<int>> mergedSectors(targetSectors);
                        for (int i = 0; i < actualSectors; i++) {
                            int targetIndex = i * targetSectors / actualSectors;
                            mergedSectors[targetIndex].insert(
                                mergedSectors[targetIndex].end(),
                                searchSpaceAreasBySolutions[i].begin(),
                                searchSpaceAreasBySolutions[i].end()
                            );
                        }
                        searchSpaceAreasBySolutions = mergedSectors;
                    }
                }

                void SearchSpaceDivisionBySearchAgent(){
                    for (size_t i = 0; i < xVectorsPerGeneration[1].size(); ++i) {
                        searchSpaceAreasBySearchAgent.push_back(std::vector<std::vector<int>>());
                        for (size_t j = 0; j < 8; ++j) {
                            searchSpaceAreasBySearchAgent[i].push_back(std::vector<int>());
                        }
                    }
                    int agent = 0;
                    for (size_t i = 0; i < searchSpaceAreasBySolutions.size(); ++i) {
                        for (size_t j = 0; j < searchSpaceAreasBySolutions[i].size(); ++j) {
                            agent = searchSpaceAreasBySolutions[i][j] % xVectorsPerGeneration[1].size();
                            searchSpaceAreasBySearchAgent[agent][i].push_back(searchSpaceAreasBySolutions[i][j]);
                        }
                    }
                    /*
                    for (size_t i = 0; i < searchSpaceAreasBySearchAgent.size(); ++i) {
                        for (size_t j = 0; j < searchSpaceAreasBySearchAgent[i].size(); ++j) {
                            std::cout << "Agent " << i << " Area " << j << ": ";
                            std::cout << "Area size: " << searchSpaceAreasBySearchAgent[i][j].size() << " ";
                            std::cout << std::endl;
                            std::cout << "Area elements: ";
                            std::cout << std::endl;
                            for (size_t k = 0; k < searchSpaceAreasBySearchAgent[i][j].size(); ++k) {
                                std::cout << searchSpaceAreasBySearchAgent[i][j][k] << " ";
                            }
                            std::cout << std::endl;
                        }
                    }*/
                }

                void SearchSpaceDivisionByGeneration(){
                    int genIndex = getIndex(dataNames, "generation");
                    int agent = 0;
                    //firstGen
                    searchSpaceAreasByGenerations.push_back(std::vector<int>());
                    for (size_t j = 0; j < this->n_firstGen; ++j) {
                        searchSpaceAreasByGenerations[0].push_back(-1);
                    }
                    for (int i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        searchSpaceAreasByGenerations.push_back(std::vector<int>());
                        for (size_t j = 0; j < this->n_children; ++j) {
                            searchSpaceAreasByGenerations[i].push_back(-1);
                        }
                    }
                    for (size_t i = 0; i < data[genIndex].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            for (size_t j = 0; j < searchSpaceAreasBySolutions.size(); ++j) {
                                if (std::find(searchSpaceAreasBySolutions[j].begin(), searchSpaceAreasBySolutions[j].end(), i) != searchSpaceAreasBySolutions[j].end()) {
                                    searchSpaceAreasByGenerations[0][agent] = j;
                                    ++agent;
                                }
                            }
                            if (agent == this->n_children){
                                agent = 0;
                            }
                        }
                        else{
                            for (size_t j = 0; j < searchSpaceAreasBySolutions.size(); ++j) {
                                if (std::find(searchSpaceAreasBySolutions[j].begin(), searchSpaceAreasBySolutions[j].end(), i) != searchSpaceAreasBySolutions[j].end()) {
                                    searchSpaceAreasByGenerations[data[genIndex][i]][agent] = j;
                                    ++agent;
                                }
                            }
                            if (agent == this->n_children){
                                agent = 0;
                            }
                        }
                    }

                    //PostProcessing
                    for (size_t i = 0; i < searchSpaceAreasByGenerations.size(); ++i) {
                        for (size_t j = 0; j < searchSpaceAreasByGenerations[i].size(); ++j) {
                            if (searchSpaceAreasByGenerations[i][j] == -1) {
                                searchSpaceAreasByGenerations[i][j] = searchSpaceAreasByGenerations[i - 1][j];
                            }
                        }
                    }
                }



                std::vector<std::vector<double>> getCrossoverImprovement(){
                    return crossoverImprovement;
                }

                std::vector<std::vector<double>> getMutationImprovement(){
                    return mutationImprovement;
                }

                template <typename T>
                struct VectorHash {
                    size_t operator()(const std::vector<T>& v) const {
                        std::hash<T> hasher;
                        size_t seed = 0;
                        for (const T& elem : v) {
                            seed ^= hasher(elem) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                        }
                        return seed;
                    }
                };
                // Custom equality comparator for vectors
                template <typename T>
                struct VectorEqual {
                    bool operator()(const std::vector<T>& a, const std::vector<T>& b) const {
                        return a == b;
                    }
                };
                // Function to get unique solutions
                template <typename T>
                std::vector<std::vector<T>> getUniqueSolutions(const std::vector<std::vector<T>>& solutions) {
                    std::unordered_set<std::vector<T>, VectorHash<T>, VectorEqual<T>> uniqueSolutions;
                    
                    for (const auto& solution : solutions) {
                        uniqueSolutions.insert(solution);
                    }
                    
                    return std::vector<std::vector<T>>(uniqueSolutions.begin(), uniqueSolutions.end());
                }
                
                double euclideanDistance(std::vector<double> x1, std::vector<double> x2){
                    double distance = 0;
                    for (size_t i = 0; i < x1.size(); ++i) {
                        distance += pow(x1[i] - x2[i], 2);
                    }
                    return sqrt(distance);
                }


            //Seccion 2: Metricas de eficiencia
                std::vector<double> objectiveValueOverTime() {
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return {};
                    }
                    int index = getIndex(dataNames, "raw_y");
                    if (index != -1) {
                        std::vector<double> objectiveValue;
                        for (size_t j = 0; j < data[index].size(); ++j) {
                            objectiveValue.push_back(data[index][j]);
                        }
                        return objectiveValue;
                    } else {
                        std::cerr << "No data" << std::endl;
                    }
                }

                std::vector<double> objectiveValueBestSolutionOverTime(){
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return {};
                    }
                    int index = getIndex(dataNames, "raw_y_best");
                    int genIndex = getIndex(dataNames, "generation");
                    if (index != -1) {
                        std::vector<double> objectiveValue;
                        for (size_t j = 0; j < data[index].size(); ++j) {
                            //Only add when there is a change in generation
                            if (j == 0 || data[genIndex][j] != data[genIndex][j - 1] || j == data[index].size() - 1) {
                                objectiveValue.push_back(data[index][j - 1]);
                            }
                        }
                        return objectiveValue;
                    } else {
                        std::cerr << "No data" << std::endl;
                    }
                }

                std::vector<double> changeInObjectiveValuePerGeneration(){
                    std::vector<double> changeInObjectiveValue;
                    double maxPrevious = 0;
                    double maxCurrent = 0;
                    for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                        maxPrevious = *std::max_element(objectiveValuePerGeneration[i - 1].begin(), objectiveValuePerGeneration[i - 1].end());
                        maxCurrent = *std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        changeInObjectiveValue.push_back(maxCurrent - maxPrevious);
                    }
                    return changeInObjectiveValue;
                }

                std::vector<std::vector<double>> rateOfChangePerIndividualPerGeneration(int optimum, int n_firstGen, int n_children){
                    //Check how many individuals are in each generation
                    std::cout << "Number of individuals in each generation: " << n_children << std::endl;
                    std::vector<std::vector<double>> objectiveValuePerInividualPerGeneration;
                    for (size_t i = 0; i < n_children; ++i) {
                        objectiveValuePerInividualPerGeneration.push_back(std::vector<double>());
                    }
                    //Compare objective value of individuals vs optimum
                    for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                        for (size_t j = 0; j < objectiveValuePerGeneration[i].size(); ++j) {
                            objectiveValuePerInividualPerGeneration[j].push_back(objectiveValuePerGeneration[i][j] - optimum);
                        }
                    }
                    
                    return objectiveValuePerInividualPerGeneration;
                }

                std::vector<double> calculateConvergenceOptimumBased(int optimum, bool maxProblem) {
                    std::vector<double> convergence;
                    if (maxProblem) {
                    for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                        double convergenceStep = 0;
                        //Use best individual in each generation
                        double max = *std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        double maxPrevious = *std::max_element(objectiveValuePerGeneration[i - 1].begin(), objectiveValuePerGeneration[i - 1].end());
                        convergenceStep = (optimum - max) / (optimum - maxPrevious);
                        convergence.push_back(convergenceStep);
                    }
                    return convergence;
                    }
                    else {
                        for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                            double convergenceStep = 0;
                            //Use best individual in each generation
                            double min = *std::min_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                            double minPrevious = *std::min_element(objectiveValuePerGeneration[i - 1].begin(), objectiveValuePerGeneration[i - 1].end());
                            convergenceStep = (min - optimum) / (minPrevious - optimum);
                            convergence.push_back(convergenceStep);
                        }
                        return convergence;
                    }
                }

                std::vector<double> classicConvergenceOptimumBased(int optimum, bool maxProblem) {
                    std::vector<double> convergence;
                    if (maxProblem) {
                        for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                            double convergenceStep = 0;
                            //Use best individual in each generation
                            double max = *std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                            convergenceStep = 1 - (optimum - max) / (optimum);
                            convergence.push_back(convergenceStep);
                        }
                    } else {
                        for (size_t i = 1; i < objectiveValuePerGeneration.size(); ++i) {
                            double convergenceStep = 0;
                            //Use best individual in each generation
                            double min = *std::min_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                            convergenceStep = 1 - (min - optimum) / (min);
                            convergence.push_back(convergenceStep);
                        }
                    }
                    return convergence;
                }
            
                std::vector<std::vector<double>> calculateConvergenceStepsPerIndividual(int n_firstGen, int n_children) {
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return {};
                    }
                    //Use objectiveValuePerGeneration
                    std::vector<std::vector<double>> convergence;
                    double convergenceStep;
                    for (size_t i = 0; i < n_children; ++i) {
                        convergence.push_back(std::vector<double>());
                    }

                    for (size_t i = 1; i < objectiveValuePerGeneration.size() - 1; ++i) {
                        for (size_t j = 0; j < n_children; ++j) {
                            if (objectiveValuePerGeneration[i][j] == objectiveValuePerGeneration[i - 1][j]) {
                                convergenceStep = 0;
                            } else {
                                convergenceStep = (objectiveValuePerGeneration[i+1][j] - objectiveValuePerGeneration[i][j]) / (objectiveValuePerGeneration[i][j] - objectiveValuePerGeneration[i - 1][j]);
                            }
                            
                            convergence[j].push_back(convergenceStep);
                        }
                    }
                    return convergence;
                }

                std::vector<double> calculateConvergenceSteps() {
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return {};
                    }
                    //Use objectiveValuePerGeneration
                    std::vector<double> convergence;
                    double convergenceStep;
                    for (size_t i = 0; i < objectiveValuePerGeneration.size()-1; ++i) {
                        double convergenceStep = 0;
                        //Use best individual in each generation
                        double max = *std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        double maxNext = *std::max_element(objectiveValuePerGeneration[i + 1].begin(), objectiveValuePerGeneration[i + 1].end());
                        convergenceStep = (maxNext - max) / (max);
                        convergence.push_back(convergenceStep);
                    }
                    return convergence;
                }

                std::vector<double> geometricRateofFitnessChangePerGeneration(int optimum, int n_firstGen, int n_children){
                    //Se hace por raw_y_best por generacion
                    //Y tambien se tiene que ahcer por individuo.
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return {};
                    }
                    int index = getIndex(dataNames, "raw_y_best");
                    int genIndex = getIndex(dataNames, "generation");

                    if (index != -1) {
                        std::vector<double> geomFitnessChange;
                        double geomChangeGen = 0;
                        for (size_t i = 1; i < data[genIndex].size() + 1; ++i) {
                            if(data[genIndex][i] != data[genIndex][i - 1]){
                                double x = (optimum - data[index][i-1]) / (optimum - data[index][0]);
                                geomChangeGen = 1 - pow(x, 1.0/data[genIndex][i-1]);
                                geomFitnessChange.push_back(geomChangeGen);
                            }   
                        }
                        return geomFitnessChange;
                    } else {
                        std::cerr << "No data" << std::endl;
                    }
                }

                /*double e_value(int optimum){ //Con cantidad de evaluaciones
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return -1;
                    }

                    double desiredValue = optimum*0.90;
                    double n_successful = 0;
                    double n_sim = 0;
                    for (size_t i = 0; i < objectiveValuePerGeneration.size(); ++i) {
                            for (size_t j = 0; j < objectiveValuePerGeneration[i].size(); ++j) {
                                if (objectiveValuePerGeneration[i][j] >= desiredValue) {
                                    ++n_successful;
                                }
                                ++n_sim;
                            } 
                        }
                    double n_quality = n_successful / n_sim;

                    double n_convergence = static_cast<double>(data[0][data[0].size() - 1]) / (nCr(n_var, 2));

                    double e_ = n_quality / n_convergence;
                    return e_;
                }*/

                //Implementacion nueva
                double calculate_n_quality(int optimum) {
                    if (dataNames.empty()) {
                        std::cerr << "No data available" << std::endl;
                        return -1;
                    }
                    
                    double desiredValue = optimum * 0.90;
                    double n_successful = 0;
                    double n_sim = 0;
                    for (size_t i = 0; i < objectiveValuePerGeneration.size(); ++i) {
                        for (size_t j = 0; j < objectiveValuePerGeneration[i].size(); ++j) {
                            if (objectiveValuePerGeneration[i][j] >= desiredValue) {
                                ++n_successful;
                            }
                            ++n_sim;
                        } 
                    }
                    return n_successful / n_sim;
                }

                double calculate_n_convergence() {
                    std::vector<std::vector<double>> xVectors;
                    for (size_t i = 0; i < data[0].size(); ++i) {
                        xVectors.push_back(std::vector<double>());
                        for (size_t j = 0; j < n_var; ++j) {
                            xVectors[i].push_back(data[getIndex(dataNames, "x" + std::to_string(j))][i]);
                        }
                    }
                    auto uniqueSolutions = getUniqueSolutions(xVectors);
                    return static_cast<double>(uniqueSolutions.size()) / (nCr(n_var, 2));
                }

                double e_value(int optimum) {
                    double n_quality = calculate_n_quality(optimum);
                    double n_convergence = calculate_n_convergence();
                    
                    return n_quality / n_convergence;
                }
                double e_value(double n_quality, double n_convergence) {
                    return n_quality / n_convergence;
                }

                //Subsection: Hamming
                //Hamming distance between best solution in the first generation and the best solution in the current generation
                std::vector<double> hammingDistanceBestSolutionPerGeneration(){
                    std::vector<double> hammingDistances;
                    for (size_t i = 1; i < xVectorsProcessed.size(); ++i) {
                        size_t maxIndex = std::distance(objectiveValuePerGeneration[i].begin(), 
                                        std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end()));
                        hammingDistances.push_back(hammingDistance(xVectorsProcessed[0][0], xVectorsProcessed[i][maxIndex]));
                    }
                    return hammingDistances;
                }

                //Hamming distance between best solution in the current generation and the best solution in the previous generation
                std::vector<double> hammingDistanceBestSolutionPerGeneration2(){
                    std::vector<double> hammingDistances;
                    for (size_t i = 1; i < xVectorsProcessed.size(); ++i) {
                        size_t maxIndex = std::distance(objectiveValuePerGeneration[i].begin(), 
                                        std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end()));
                        size_t maxIndexPrevious = std::distance(objectiveValuePerGeneration[i - 1].begin(), 
                                        std::max_element(objectiveValuePerGeneration[i - 1].begin(), objectiveValuePerGeneration[i - 1].end()));
                        hammingDistances.push_back(hammingDistance(xVectorsProcessed[i - 1][maxIndexPrevious], xVectorsProcessed[i][maxIndex]));
                    }
                    return hammingDistances;
                }

                //Hamming distance between best global solution and the best solution in the current generation
                std::vector<double> hammingDistanceBestSolutionPerGeneration3(){
                    std::vector<double> hammingDistances;
                    std::vector<double> bestGlobalSolution;

                    int index = getIndex(dataNames, "raw_y_best");
                    //get the best in the first generation
                    size_t maxIndex = std::distance(objectiveValuePerGeneration[0].begin(), 
                                        std::max_element(objectiveValuePerGeneration[0].begin(), objectiveValuePerGeneration[0].end()));
                    bestGlobalSolution = xVectorsProcessed[0][maxIndex];
                    hammingDistances.push_back(0);
                    for (size_t i = 1; i < xVectorsProcessed.size(); ++i) {
                        size_t maxIndex = std::distance(objectiveValuePerGeneration[i].begin(), 
                                        std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end()));
                        if (data[index][i*n_children] > data[index][i*n_children - 1]) {
                            bestGlobalSolution = xVectorsProcessed[i][maxIndex];
                        }
                        hammingDistances.push_back(hammingDistance(bestGlobalSolution, xVectorsProcessed[i][maxIndex]));
                    }
                    return hammingDistances;
                }

                std::vector<double> entropyDiversity(){
                    SearchSpaceDivisionByGeneration();
                    double diversity = 0;
                    double areaScores[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                    std::vector<double> areaScoresVector;
                    for (size_t i = 0; i < searchSpaceAreasByGenerations.size(); ++i) {
                        diversity = 0;
                        //reset areaScores
                        for (size_t j = 0; j < 8; ++j) {
                            areaScores[j] = 0;
                        }
                        for (size_t j = 0; j < searchSpaceAreasByGenerations[i].size(); ++j) {
                            areaScores[searchSpaceAreasByGenerations[i][j]] += 1;
                        }
                        for (size_t j = 0; j < 8; ++j) {
                            //std::cout << "Area " << j << " Score: " << areaScores[j] << std::endl;
                            if (areaScores[j] != 0){
                                diversity += (areaScores[j] / n_children) * log(areaScores[j] / n_children);
                            }
                        }
                        areaScoresVector.push_back(-diversity);
                    }
                    return areaScoresVector;
                }

                //Incluir mdsCustomAreas
                std::vector<double> entropyDiversityCustomAreas(int n_areas){
                    SearchSpaceDivisionByGeneration();
                    double diversity = 0;
                    std::vector<double> areaScoresVector;
                    for (size_t i = 0; i < searchSpaceAreasByGenerations.size(); ++i) {
                        diversity = 0;
                        std::vector<double> areaScores(n_areas, 0);
                        for (size_t j = 0; j < searchSpaceAreasByGenerations[i].size(); ++j) {
                            areaScores[searchSpaceAreasByGenerations[i][j]] += 1;
                        }
                        for (size_t j = 0; j < n_areas; ++j) {
                            if (areaScores[j] != 0){
                                diversity += (areaScores[j] / n_children) * log(areaScores[j] / n_children);
                            }
                        }
                        areaScoresVector.push_back(-diversity);
                    }
                    return areaScoresVector;
                }

                std::vector<std::vector<double>> diversityDistanceToCenter(int R){ //Without best individual as center (0 as center)
                    double distance = 0;
                    std::vector<double> segments;
                    std::vector<std::vector<double>> diversity;
                    double rMax = std::sqrt(n_var); //Has to change in case of non binary variables
                    for (int i = 0; i < R; ++i) {
                        segments.push_back((i*rMax)/R);
                    }
                    segments.push_back(rMax);
                    for (size_t i = 0; i < xVectorsProcessed.size(); ++i) {
                        diversity.push_back(std::vector<double>());
                        for (size_t j = 0; j < xVectorsProcessed[i].size(); ++j) {
                            //Use euclidean distance with x2 as 0
                            distance = euclideanDistance(xVectorsProcessed[i][j], std::vector<double>(n_var, 0));
                            for (int k = 0; k < R; ++k) {
                                if (distance >= segments[k] && distance < segments[k + 1]) {
                                    diversity[i].push_back(k);
                                    break;
                                }
                            }
                        }
                    }
                    return diversity;
                }

                std::vector<std::vector<double>> diversityDistanceToCenter2(int R){ //With best individual as center
                    double distance = 0;
                    std::vector<double> segments;
                    std::vector<std::vector<double>> diversity;
                    double rMax = std::sqrt(n_var); //Has to change in case of non binary variables
                    for (int i = 0; i < R; ++i) {
                        segments.push_back((i*rMax)/R); //Recalculate on every iteration
                    }
                    segments.push_back(rMax);
                    for (size_t i = 0; i < xVectorsProcessed.size(); ++i) {
                        diversity.push_back(std::vector<double>());
                        //Use euclidean distance with x2 as best individual in the generation
                        auto maxIt = std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        size_t maxIndex = std::distance(objectiveValuePerGeneration[i].begin(), maxIt);
                        for (size_t j = 0; j < xVectorsProcessed[i].size(); ++j) {
                            distance = euclideanDistance(xVectorsProcessed[i][j], xVectorsProcessed[i][maxIndex]);
                            for (int k = 0; k < R; ++k) {
                                
                                if (distance >= segments[k] && distance < segments[k + 1]) {
                                    diversity[i].push_back(k);
                                    break;
                                }
                            }
                        }
                    }
                    return diversity;
                }

                std::vector<double> accumSum(bool maxProblem){
                    std::vector<double> accumSumVector;
                    double sum = 0;
                    double max, previousMax;
                    accumSumVector.push_back(0);
                    for (size_t i = 0; i < objectiveValuePerGeneration.size(); ++i) {
                        //Use best individual in each generation
                        if (maxProblem)
                            max = *std::max_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        else
                            max = *std::min_element(objectiveValuePerGeneration[i].begin(), objectiveValuePerGeneration[i].end());
                        //If max is better than previous max, add 1 to sum and push sum to accumSumVector
                        //Else, add -1 to sum and push sum to accumSumVector
                        if (i == 0) {
                            previousMax = max;
                        } else {
                            if (maxProblem) {
                                if (max > previousMax) {
                                    sum += 1;
                                } 
                                else if (max < previousMax) {
                                    sum += -1;
                                }
                                else {
                                    sum += 0;
                                }
                            } else {
                                if (max < previousMax) {
                                    sum += 1;
                                } 
                                else if (max > previousMax) {
                                    sum += -1;
                                }
                                else {
                                    sum += 0;
                                }
                            }
                            accumSumVector.push_back(sum);
                            previousMax = max;
                        }
                    }
                    return accumSumVector;
                }

                /*
                void PSODiversity(){
                    //D_swarmCenter = 1/|S| * sum_{i=1}^|S| (sqrt(sum_{j=1}^D (x_ij - x_j)^2)) with x_j the j-th component of the swarm center -> Swarm center is the average of the swarm
                    D = n_var; //Problem dimension
                    S_ = n_children; //Swarm size
                    int genIndex = getIndex(dataNames, "generation");
                    int index = getIndex(dataNames, "x0");
                    std::vector<std::vector<double>> swarmCenter;
                    for (size_t i = 0; i < data[genIndex].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            swarmCenter.push_back(std::vector<double>());
                            for (int j = 0; j < n_var; ++j) {
                                swarmCenter[0].push_back(data[index + j][i]);
                            }
                        } else {
                            for (int j = 0; j < n_var; ++j) {
                                swarmCenter[data[genIndex][i]].push_back(data[index + j][i]);
                            }
                        }
                    }
                }*/
            //Seccion 3: Efectividad
            //Seleccionar -> Quiza profiles of measure
            //Para estas, las utiles usan mas de una run del algoritmo, consultar si haremos eso.
            //Seccion 4: Operadores
                //Metrica sobre rate de success por operador
                //Metricas sobre rate de mejora por operador 

                std::tuple<double,double> rateOfModificationsPerOperator(){ //Success = Change in objective value != better objective value
                    double rateOfModificationsMutations = 0;
                    double rateOfModificationsCrossovers = 0;

                    int totalMutations = data[getIndex(dataNames, "totalMutations")][data[getIndex(dataNames, "totalMutations")].size() - 1];
                    int totalCrossovers = data[getIndex(dataNames, "totalCrossovers")][data[getIndex(dataNames, "totalCrossovers")].size() - 1];
                    int successfulMutations = data[getIndex(dataNames, "successfulMutations")][data[getIndex(dataNames, "successfulMutations")].size() - 1];
                    int successfulCrossovers = data[getIndex(dataNames, "successfulCrossovers")][data[getIndex(dataNames, "successfulCrossovers")].size() - 1];

                    rateOfModificationsMutations = (double) successfulMutations / totalMutations;
                    rateOfModificationsCrossovers = (double) successfulCrossovers / totalCrossovers;

                    return std::make_tuple(rateOfModificationsMutations, rateOfModificationsCrossovers);
                }

                double rateOfModificationsCrossovers(){
                    double rateOfModificationsCrossovers = 0;
                    int totalCrossovers = data[getIndex(dataNames, "totalCrossovers")][data[getIndex(dataNames, "totalCrossovers")].size() - 1];
                    int successfulCrossovers = data[getIndex(dataNames, "successfulCrossovers")][data[getIndex(dataNames, "successfulCrossovers")].size() - 1];
                    rateOfModificationsCrossovers = (double) successfulCrossovers / totalCrossovers;
                    return rateOfModificationsCrossovers;
                }

                double rateOfModificationsMutations(){
                    double rateOfModificationsMutations = 0;
                    int totalMutations = data[getIndex(dataNames, "totalMutations")][data[getIndex(dataNames, "totalMutations")].size() - 1];
                    int successfulMutations = data[getIndex(dataNames, "successfulMutations")][data[getIndex(dataNames, "successfulMutations")].size() - 1];
                    rateOfModificationsMutations = (double) successfulMutations / totalMutations;
                    return rateOfModificationsMutations;
                }

                double rateOfSuccessCrossovers(){
                    double rateOfSuccessCrossovers = 0;
                    int totalCrossovers = data[getIndex(dataNames, "totalCrossovers")][data[getIndex(dataNames, "totalCrossovers")].size() - 1];
                    int successfulCrossovers = data[getIndex(dataNames, "successfulCrossovers")][data[getIndex(dataNames, "successfulCrossovers")].size() - 1];
                    rateOfSuccessCrossovers = (double) successfulCrossovers / totalCrossovers;
                    return rateOfSuccessCrossovers;
                }

                double rateOfSuccessMutations(){
                    double rateOfSuccessMutations = 0;
                    int totalMutations = data[getIndex(dataNames, "totalMutations")][data[getIndex(dataNames, "totalMutations")].size() - 1];
                    int successfulMutations = data[getIndex(dataNames, "successfulMutations")][data[getIndex(dataNames, "successfulMutations")].size() - 1];
                    rateOfSuccessMutations = (double) successfulMutations / totalMutations;
                    return rateOfSuccessMutations;
                }

                double rateOfFailureCrossovers(){
                    double rateOfFailureCrossovers = 0;
                    int totalCrossovers = data[getIndex(dataNames, "totalCrossovers")][data[getIndex(dataNames, "totalCrossovers")].size() - 1];
                    int successfulCrossovers = data[getIndex(dataNames, "successfulCrossovers")][data[getIndex(dataNames, "failedCrosuccessfulCrossoversssovers")].size() - 1];
                    rateOfFailureCrossovers =   1 - (double) successfulCrossovers / totalCrossovers;
                    return rateOfFailureCrossovers;
                }

                double rateOfFailureMutations(){
                    double rateOfFailureMutations = 0;
                    int totalMutations = data[getIndex(dataNames, "totalMutations")][data[getIndex(dataNames, "totalMutations")].size() - 1];
                    int successfulMutations = data[getIndex(dataNames, "successfulMutations")][data[getIndex(dataNames, "successfulMutations")].size() - 1];
                    rateOfFailureMutations = 1 - (double) successfulMutations / totalMutations;
                    return rateOfFailureMutations;
                }

                void rateOfImprovementPerOperator(){
                    std::vector<std::vector<double>> mutationImprovement; //Generations -> improvement, agent is the position in the vector
                    std::vector<std::vector<double>> crossoverImprovement; //Generations -> improvement, agent is the position in the vector
                    
                    int successfulMutationsIndex = getIndex(dataNames, "successfulMutations");
                    int successfulCrossoversIndex = getIndex(dataNames, "successfulCrossovers");
                    int genIndex = getIndex(dataNames, "generation");
                    int index = getIndex(dataNames, "raw_y");
                    int counter = 0;

                    //FirstGen
                    mutationImprovement.push_back(std::vector<double>());
                    for (int i = 0; i < n_firstGen; ++i) {
                        mutationImprovement[0].push_back(0);
                    }

                    //Rest of generations
                    for (size_t i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        mutationImprovement.push_back(std::vector<double>());
                        for (int j = 0; j < n_children; ++j) {
                            mutationImprovement[i].push_back(0);
                        }
                    }
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            mutationImprovement[0][i] = 0;
                        } else {
                            if (data[genIndex][i] != data[genIndex][i - 1]) {
                                counter = 0;
                            }
                            if (data[successfulMutationsIndex][i] > data[successfulMutationsIndex][i - 1]){
                                mutationImprovement[data[genIndex][i]][counter] = data[index][i] - data[index][i - 1];
                            }
                            ++counter;
                        }
                    }

                    //same for crossovers
                    //FirstGen
                    crossoverImprovement.push_back(std::vector<double>());
                    for (int i = 0; i < n_firstGen; ++i) {
                        crossoverImprovement[0].push_back(0);
                    }
                    //Rest of generations
                    for (size_t i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        crossoverImprovement.push_back(std::vector<double>());
                        for (int j = 0; j < n_children; ++j) {
                            crossoverImprovement[i].push_back(0);
                        }
                    }
                    counter = 0;
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            crossoverImprovement[0][i] = 0;
                        } else {
                            if (data[genIndex][i] != data[genIndex][i - 1]) {
                                counter = 0;
                            }
                            if (data[successfulCrossoversIndex][i] > data[successfulCrossoversIndex][i - 1]){
                                crossoverImprovement[data[genIndex][i]][counter] = data[index][i] - data[index][i - 1];
                            }
                            ++counter;
                        }
                    }
                    
                    double meanImprovementMutations = 0;
                    double meanImprovementCrossovers = 0;
                    double meanDeteriorationMutations = 0;
                    double meanDeteriorationCrossovers = 0;

                    int counterImprovementMutations = 0;
                    int counterImprovementCrossovers = 0;
                    int counterDeteriorationMutations = 0;
                    int counterDeteriorationCrossovers = 0;

                    for (size_t i = 0; i < mutationImprovement.size(); ++i) {
                        for (size_t j = 0; j < mutationImprovement[i].size(); ++j) {
                            if (mutationImprovement[i][j] > 0){
                                meanImprovementMutations += mutationImprovement[i][j];
                                ++counterImprovementMutations;
                            } else if (mutationImprovement[i][j] < 0){
                                meanDeteriorationMutations += mutationImprovement[i][j];
                                ++counterDeteriorationMutations;
                            }
                        }
                    }

                    for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
                        for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
                            if (crossoverImprovement[i][j] > 0){
                                meanImprovementCrossovers += crossoverImprovement[i][j];
                                ++counterImprovementCrossovers;
                            } else if (crossoverImprovement[i][j] < 0){
                                meanDeteriorationCrossovers += crossoverImprovement[i][j];
                                ++counterDeteriorationCrossovers;
                            }
                        }
                    }

                    meanImprovementMutations /= counterImprovementMutations;
                    meanImprovementCrossovers /= counterImprovementCrossovers;

                    meanDeteriorationMutations /= counterDeteriorationMutations;
                    meanDeteriorationCrossovers /= counterDeteriorationCrossovers;
                }

                std::tuple<double,double> rateOfImprovementMutations(){
                    //Generations -> improvement, agent is the position in the vector
                    int successfulMutationsIndex = getIndex(dataNames, "successfulMutations");
                    int genIndex = getIndex(dataNames, "generation");
                    int index = getIndex(dataNames, "raw_y");
                    int counter = 0;

                    //FirstGen
                    mutationImprovement.push_back(std::vector<double>());
                    for (int i = 0; i < n_firstGen; ++i) {
                        mutationImprovement[0].push_back(0);
                    }

                    //Rest of generations
                    for (size_t i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        mutationImprovement.push_back(std::vector<double>());
                        for (int j = 0; j < n_children; ++j) {
                            mutationImprovement[i].push_back(0);
                        }
                    }
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            mutationImprovement[0][i] = 0;
                        } else {
                            if (data[genIndex][i] != data[genIndex][i - 1]) {
                                counter = 0;
                            }
                            if (data[successfulMutationsIndex][i] > data[successfulMutationsIndex][i - 1]){
                                mutationImprovement[data[genIndex][i]][counter] = data[index][i] - data[index][i - 1];
                            }
                            ++counter;
                        }
                    }

                    double meanImprovementMutations = 0;
                    double meanDeteriorationMutations = 0;

                    int counterImprovementMutations = 0;
                    int counterDeteriorationMutations = 0;

                    for (size_t i = 0; i < mutationImprovement.size(); ++i) {
                        for (size_t j = 0; j < mutationImprovement[i].size(); ++j) {
                            if (mutationImprovement[i][j] > 0){
                                meanImprovementMutations += mutationImprovement[i][j];
                                ++counterImprovementMutations;
                            } else if (mutationImprovement[i][j] < 0){
                                meanDeteriorationMutations += mutationImprovement[i][j];
                                ++counterDeteriorationMutations;
                            }
                        }
                    }

                    meanImprovementMutations /= counterImprovementMutations;
                    meanDeteriorationMutations /= counterDeteriorationMutations;

                    return std::make_tuple(meanImprovementMutations, meanDeteriorationMutations);
                }

                std::tuple<double,double> rateOfImprovementCrossovers(){
                    //Generations -> improvement, agent is the position in the vector
                    int successfulCrossoversIndex = getIndex(dataNames, "successfulCrossovers");
                    int genIndex = getIndex(dataNames, "generation");
                    int index = getIndex(dataNames, "raw_y");
                    int counter = 0;

                    //FirstGen
                    crossoverImprovement.push_back(std::vector<double>());
                    for (int i = 0; i < n_firstGen; ++i) {
                        crossoverImprovement[0].push_back(0);
                    }

                    //Rest of generations
                    for (size_t i = 1; i <= data[genIndex][data[genIndex].size() - 1]; ++i) {
                        crossoverImprovement.push_back(std::vector<double>());
                        for (int j = 0; j < n_children; ++j) {
                            crossoverImprovement[i].push_back(0);
                        }
                    }
                    for (size_t i = 0; i < data[index].size(); ++i) {
                        if (data[genIndex][i] == 0) {
                            crossoverImprovement[0][i] = 0;
                        } else {
                            if (data[genIndex][i] != data[genIndex][i - 1]) {
                                counter = 0;
                            }
                            if (data[successfulCrossoversIndex][i] > data[successfulCrossoversIndex][i - 1]){
                                crossoverImprovement[data[genIndex][i]][counter] = data[index][i] - data[index][i - 1];
                            }
                            ++counter;
                        }
                    }

                    double meanImprovementCrossovers = 0;
                    double meanDeteriorationCrossovers = 0;

                    int counterImprovementCrossovers = 0;
                    int counterDeteriorationCrossovers = 0;

                    for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
                        for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
                            if (crossoverImprovement[i][j] > 0){
                                meanImprovementCrossovers += crossoverImprovement[i][j];
                                ++counterImprovementCrossovers;
                            } else if (crossoverImprovement[i][j] < 0){
                                meanDeteriorationCrossovers += crossoverImprovement[i][j];
                                ++counterDeteriorationCrossovers;
                            }
                        }
                    }

                    meanImprovementCrossovers /= counterImprovementCrossovers;
                    meanDeteriorationCrossovers /= counterDeteriorationCrossovers;

                    return std::make_tuple(meanImprovementCrossovers, meanDeteriorationCrossovers);
                }
        };
    }
}
