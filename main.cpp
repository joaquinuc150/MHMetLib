#include "metrics/metrics.hpp"
#include "metrics/efficiency.hpp"
#include "metrics/effectiveness.hpp"
#include "reader_logger/read_logger.hpp"
#include "reader_logger/output_logger.hpp"
#include "utils/prints.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

ioh::common::MetricsAnalyzer prepareData(ioh::common::MetricsAnalyzer metricsAnalyzer, std::string fileName, int n_var, int n_firstGen, int n_children, int optimum, int entropyZones) {
    std::cout << "Reading data from file" << std::endl;
    metricsAnalyzer.readDataFromFile(fileName); //Get standard file name to read correctly
    std::cout << "crateXVectorsPerEval" << std::endl;
    metricsAnalyzer.createXVectorsPerEvaluation(n_var);
    std::cout << "createXMatrixPerGen" << std::endl;
    metricsAnalyzer.createXVectorsPerGeneration();
    std::cout << "XVectorsProcessed" << std::endl;
    metricsAnalyzer.postProcessGenerations();
    std::cout << "createObjectiveValuePerGen" << std::endl;
    metricsAnalyzer.createObjectiveValuePerGeneration();

    std::cout << "MDS" << std::endl;
    metricsAnalyzer.MDSSearchSpaceDivisionCustomSectors(entropyZones);

    return metricsAnalyzer;
}

void createFilesForPlots(ioh::common::MetricsAnalyzer metricsAnalyzer, int n_firstGen, int n_children, int optimum, int entropyZones, int R, bool maxProblem, std::string route = "textData/"){
    fs::path base_path = fs::current_path() / route;
    if (!fs::exists(base_path)) {
        fs::create_directories(base_path);
    }
    
    std::cout << "objectiveValueOverTime" << std::endl;
    //plot objectiveValueOverTime
    std::cout << route + "objectiveValueOverTime.txt";
    std::ofstream file1(route + "objectiveValueOverTime.txt");
    std::vector<double> objectiveValueOverTimeVector = metricsAnalyzer.objectiveValueOverTime();
    if (file1.is_open()) {
        for (size_t i = 0; i < objectiveValueOverTimeVector.size(); ++i) {
            file1 << i << " " << objectiveValueOverTimeVector[i] << std::endl;
        }
        file1.close();
    } else {
        std::cerr << "Failed to open data file: " << "objectiveValueOverTime.txt" << std::endl;
    }

    std::cout << "objectiveValueBestSolutionOverTime" << std::endl;
    //plot objectiveValueBestSolutionOverTime
    std::ofstream file13(route + "objectiveValueBestSolutionOverTime.txt");
    std::vector<double> objectiveValueBestSolutionOverTimeVector = metricsAnalyzer.objectiveValueBestSolutionOverTime();
    if (file13.is_open()) {
        for (size_t i = 0; i < objectiveValueBestSolutionOverTimeVector.size(); ++i) {
            file13 << i << " " << objectiveValueBestSolutionOverTimeVector[i] << std::endl;
        }
        file13.close();
    } else {
        std::cerr << "Failed to open data file: " << "objectiveValueBestSolutionOverTime.txt" << std::endl;
    }

    std::cout << "changeInObjectiveValuePerGeneration" << std::endl;
    //plot ChangeInObjectiveValuePerGeneration
    std::ofstream file2(route + "changeInObjectiveValuePerGeneration.txt");
    std::vector<double> changeInObjectiveValuePerGenerationVector = metricsAnalyzer.changeInObjectiveValuePerGeneration();
    if (file2.is_open()) {
        for (size_t i = 0; i < changeInObjectiveValuePerGenerationVector.size(); ++i) {
            file2 << i << " " << changeInObjectiveValuePerGenerationVector[i] << std::endl;
        }
        file2.close();
    } else {
        std::cerr << "Failed to open data file: " << "changeInObjectiveValuePerGeneration.txt" << std::endl;
    }

    std::cout << "rateOfChangePerIndividualPerGeneration" << std::endl;
    //plot rateOfChangePerIndividualPerGeneration
    std::ofstream file3(route + "rateOfChangePerIndividualPerGeneration.txt");
    std::vector<std::vector<double>> rateOfChangePerIndividualPerGenerationVector = metricsAnalyzer.rateOfChangePerIndividualPerGeneration(optimum, n_firstGen, n_children);
    if (file3.is_open()) {
        for (size_t i = 0; i < rateOfChangePerIndividualPerGenerationVector.size(); ++i) {
            for (size_t j = 0; j < rateOfChangePerIndividualPerGenerationVector[i].size(); ++j) {
                file3 << rateOfChangePerIndividualPerGenerationVector[i][j] << " ";
            }
            file3 << std::endl;
        }
        file3.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfChangePerIndividualPerGeneration.txt" << std::endl;
    }

    std::cout << "convergenceOptimumBased" << std::endl;
    //plot convergenceOptimumBased
    std::ofstream file4(route + "convergenceOptimumBased.txt");
    std::vector<double> convergenceOptimumBasedVector = metricsAnalyzer.calculateConvergenceOptimumBased(optimum, maxProblem);
    if (file4.is_open()) {
        for (size_t i = 0; i < convergenceOptimumBasedVector.size(); ++i) {
            file4 << i << " " << convergenceOptimumBasedVector[i] << std::endl;
        }
        file4.close();
    } else {
        std::cerr << "Failed to open data file: " << "convergenceOptimumBased.txt" << std::endl;
    }

    std::cout << "classicConvergenceOptimumBased" << std::endl;
    //plot classicConvergenceOptimumBased
    std::ofstream file14(route + "classicConvergenceOptimumBased.txt");
    std::vector<double> classicConvergenceOptimumBasedVector = metricsAnalyzer.classicConvergenceOptimumBased(optimum, maxProblem);
    if (file14.is_open()) {
        for (size_t i = 0; i < classicConvergenceOptimumBasedVector.size(); ++i) {
            file14 << i << " " << classicConvergenceOptimumBasedVector[i] << std::endl;
        }
        file14.close();
    } else {
        std::cerr << "Failed to open data file: " << "classicConvergenceOptimumBased.txt" << std::endl;
    }

    std::cout << "convergenceStepsPerIndividual" << std::endl;
    //plot convergenceSteps
    std::ofstream file5(route + "convergenceStepsPerIndividual.txt");
    std::vector<std::vector<double>> convergenceStepsVector = metricsAnalyzer.calculateConvergenceStepsPerIndividual(n_firstGen, n_children);
    if (file5.is_open()) {
        for (size_t i = 0; i < convergenceStepsVector.size(); ++i) {
            for (size_t j = 0; j < convergenceStepsVector[i].size(); ++j) {
                file5 << convergenceStepsVector[i][j] << " ";
            }
            file5 << std::endl;
        }
        file5.close();
    } else {
        std::cerr << "Failed to open data file: " << "convergenceSteps.txt" << std::endl;
    }

    std::cout << "convergenceSteps" << std::endl;
    //plot convergenceSteps
    std::ofstream file17(route + "convergenceSteps2.txt");
    std::vector<double> convergenceStepsVector2 = metricsAnalyzer.calculateConvergenceSteps();
    if (file17.is_open()) {
        for (size_t i = 0; i < convergenceStepsVector2.size(); ++i) {
            file17 << i << " " << convergenceStepsVector2[i] << std::endl;
        }
        file17.close();
    } else {
        std::cerr << "Failed to open data file: " << "convergenceSteps2.txt" << std::endl;
    }

    std::cout << "geometricRateofFitnessChangePerGeneration" << std::endl;
    //plot geometricRateofFitnessChangePerGeneration
    std::ofstream file6(route + "geometricRateofFitnessChangePerGeneration.txt");
    std::vector<double> geometricRateofFitnessChangePerGenerationVector = metricsAnalyzer.geometricRateofFitnessChangePerGeneration(optimum, n_firstGen, n_children);
    if (file6.is_open()) {
        for (size_t i = 0; i < geometricRateofFitnessChangePerGenerationVector.size(); ++i) {
            if (i == 0){
                file6 << i << " " << 0 << std::endl;
            } else {
                file6 << i << " " << geometricRateofFitnessChangePerGenerationVector[i] << std::endl;
            }
        }
        file6.close();
    } else {
        std::cerr << "Failed to open data file: " << "geometricRateofFitnessChangePerGeneration.txt" << std::endl;
    }
    
    std::cout << "e_value" << std::endl;
    //plot e_value
    std::ofstream file15(route + "e_value.txt");
    double n_convergence = metricsAnalyzer.calculate_n_convergence();
    double n_quality = metricsAnalyzer.calculate_n_quality(optimum);
    double eValue = metricsAnalyzer.e_value(n_quality, n_convergence);
    std::cout << "eValue: " << eValue << std::endl;
    if (file15.is_open()) {
        file15 << n_quality << std::endl;
        file15 << n_convergence << std::endl;
        file15 << eValue << std::endl;
        file15.close();
    } else {
        std::cerr << "Failed to open data file: " << "e_value.txt" << std::endl;
    }
    
    std::cout << "hammingDistanceBestSolutionPerGeneration" << std::endl;
    //plot hammingDistanceBestSolutionPerGeneration
    std::ofstream file18(route + "hammingDistanceBestSolutionPerGeneration.txt");
    std::vector<double> hammingDistanceBestSolutionPerGenerationVector = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration();
    if (file18.is_open()) {
        for (size_t i = 0; i < hammingDistanceBestSolutionPerGenerationVector.size(); ++i) {
            file18 << i << " " << hammingDistanceBestSolutionPerGenerationVector[i] << std::endl;
        }
        file18.close();
    } else {
        std::cerr << "Failed to open data file: " << "hammingDistanceBestSolutionPerGeneration.txt" << std::endl;
    }

    std::cout << "hammingDistanceBestSolutionPerGeneration2" << std::endl;
    //plot hammingDistanceBestSolutionPerGeneration
    std::ofstream file19(route + "hammingDistanceBestSolutionPerGeneration2.txt");
    std::vector<double> hammingDistanceBestSolutionPerGenerationVector2 = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration2();
    if (file19.is_open()) {
        for (size_t i = 0; i < hammingDistanceBestSolutionPerGenerationVector2.size(); ++i) {
            file19 << i << " " << hammingDistanceBestSolutionPerGenerationVector2[i] << std::endl;
        }
        file19.close();
    } else {
        std::cerr << "Failed to open data file: " << "hammingDistanceBestSolutionPerGeneration2.txt" << std::endl;
    }

    std::cout << "hammingDistanceBestSolutionPerGeneration3" << std::endl;
    //plot hammingDistanceBestSolutionPerGeneration
    std::ofstream file20(route + "hammingDistanceBestSolutionPerGeneration3.txt");
    std::vector<double> hammingDistanceBestSolutionPerGenerationVector3 = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration3();
    if (file20.is_open()) {
        for (size_t i = 0; i < hammingDistanceBestSolutionPerGenerationVector3.size(); ++i) {
            file20 << i << " " << hammingDistanceBestSolutionPerGenerationVector3[i] << std::endl;
        }
        file20.close();
    } else {
        std::cerr << "Failed to open data file: " << "hammingDistanceBestSolutionPerGeneration3.txt" << std::endl;
    }

    std::cout << "entropyDiversity" << std::endl;
    //plot entropy diversity for each generation
    std::ofstream file7(route + "entropyDiversity.txt");
    std::vector<double> areaScoresVector = metricsAnalyzer.entropyDiversityCustomAreas(entropyZones);
    if (file7.is_open()) {
        for (size_t i = 0; i < areaScoresVector.size(); ++i) {
            file7 << i << " " << areaScoresVector[i] << std::endl;
        }
        file7.close();
    } else {
        std::cerr << "Failed to open data file: " << "entropyDiversity.txt" << std::endl;
    }

    std::cout << "diversityDistanceToCenter" << std::endl;
    //plot diversity distance to center for each generation
    std::ofstream file21(route + "diversityDistanceToCenter.txt");
    std::vector<std::vector<double>> diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenter(R);
    if (file21.is_open()) {
        for (size_t i = 0; i < diversityDistanceToCenterVector.size(); ++i) {
            for (size_t j = 0; j < diversityDistanceToCenterVector[i].size(); ++j) {
                file21 << diversityDistanceToCenterVector[i][j] << " ";
            }
            file21 << std::endl;
        }
        file21.close();
    } else {
        std::cerr << "Failed to open data file: " << "diversityDistanceToCenter.txt" << std::endl;
    }

    std::cout << "diversityDistanceToCenter2" << std::endl;
    //plot diversity distance to center for each generation
    std::ofstream file22(route + "diversityDistanceToCenter2.txt");
    std::vector<std::vector<double>> diversityDistanceToCenterVector2 = metricsAnalyzer.diversityDistanceToCenter2(R);
    if (file22.is_open()) {
        for (size_t i = 0; i < diversityDistanceToCenterVector2.size(); ++i) {
            for (size_t j = 0; j < diversityDistanceToCenterVector2[i].size(); ++j) {
                file22 << diversityDistanceToCenterVector2[i][j] << " ";
            }
            file22 << std::endl;
        }
        file22.close();
    } else {
        std::cerr << "Failed to open data file: " << "diversityDistanceToCenter2.txt" << std::endl;
    }

    std::cout << "accumSum" << std::endl;
    //plot accumSum
    std::ofstream file23(route + "accumSum.txt");
    std::vector<double> accumSumVector = metricsAnalyzer.accumSum(maxProblem);
    if (file23.is_open()) {
        for (size_t i = 0; i < accumSumVector.size(); ++i) {
            file23 << i << " " << accumSumVector[i] << std::endl;
        }
        file23.close();
    } else {
        std::cerr << "Failed to open data file: " << "accumSum.txt" << std::endl;
    }

    
    std::cout << "rateOfImprovementMutations" << std::endl;
    std::tuple<double,double> rateOfImprovementMutationsTuple = metricsAnalyzer.rateOfImprovementMutations();
    std::tuple<double,double> rateOfImprovementCrossoversTuple = metricsAnalyzer.rateOfImprovementCrossovers();

    std::vector<std::vector<double>> mutationImprovement = metricsAnalyzer.getMutationImprovement();
    std::vector<std::vector<double>> crossoverImprovement = metricsAnalyzer.getCrossoverImprovement();

    double meanImprovementMutations = std::get<0>(rateOfImprovementMutationsTuple);
    double meanDeteriorationMutations = std::get<1>(rateOfImprovementMutationsTuple);
    double meanImprovementCrossovers = std::get<0>(rateOfImprovementCrossoversTuple);
    double meanDeteriorationCrossovers = std::get<1>(rateOfImprovementCrossoversTuple);

    std::tuple<double,double> rateOfModificationsTuple = metricsAnalyzer.rateOfModificationsPerOperator();
    double rateOfModificationsMutations = std::get<0>(rateOfModificationsTuple);
    double rateOfModificationsCrossovers = std::get<1>(rateOfModificationsTuple);

    std::cout << "rateOfModificationsPerOperator" << std::endl;
    //plot rateOfModifications mutations and crossovers
    std::ofstream file16(route + "rateOfModificationsPerOperator.txt");
    if (file16.is_open()) {
        file16 << rateOfModificationsMutations << std::endl;
        file16 << rateOfModificationsCrossovers << std::endl;
        file16.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfModificationsPerOperator.txt" << std::endl;
    }

    std::ofstream file8(route + "rateOfImprovementMutationsAndCrossovers.txt");
    if (file8.is_open()) {
        file8 << meanImprovementMutations << std::endl;
        file8 << meanDeteriorationMutations << std::endl;
        file8 << meanImprovementCrossovers << std::endl;
        file8 << meanDeteriorationCrossovers << std::endl;
        file8.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfImprovementMutationsAndCrossovers.txt" << std::endl;
    }
    
    std::ofstream file9(route + "rateOfImprovementMutations.txt");
    if (file9.is_open()) {
        for (size_t i = 0; i < mutationImprovement.size(); ++i) {
            for (size_t j = 0; j < mutationImprovement[i].size(); ++j) {
                //Only include improvements
                if (mutationImprovement[i][j] > 0){
                    file9 << mutationImprovement[i][j] << " ";
                }
            }
        }
        file9.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfImprovementMutations.txt" << std::endl;
    }

    std::cout << "rateOfImprovementCrossovers" << std::endl;
    //plot rateofimprovement crossovers (mean improvement and mean deterioration) with boxplots
    
    std::ofstream file10(route + "rateOfImprovementCrossovers.txt");
    if (file10.is_open()) {
        for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
            for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
                //Only include improvements
                if (crossoverImprovement[i][j] > 0){
                    file10 << crossoverImprovement[i][j] << " ";
                }
            }
        }
        file10.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfImprovementCrossovers.txt" << std::endl;
    }

    std::cout << "rateOfDetoriationMutations" << std::endl;
    //plot rateofimprovement mutations (mean improvement and mean deterioration) with boxplots
    std::ofstream file11(route + "rateOfDeteriorationMutations.txt");
    if (file11.is_open()) {
        for (size_t i = 0; i < mutationImprovement.size(); ++i) {
            for (size_t j = 0; j < mutationImprovement[i].size(); ++j) {
                //Only include deteriorations
                if (mutationImprovement[i][j] < 0){
                    file11 << mutationImprovement[i][j] << " ";
                }
            }
        }
        file11.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfDeteriorationMutations.txt" << std::endl;
    }

    std::cout << "rateOfDeteriorationCrossovers" << std::endl;
    //plot rateofimprovement crossovers (mean improvement and mean deterioration) with boxplots
    std::ofstream file12(route + "rateOfDeteriorationCrossovers.txt");
    if (file12.is_open()) {
        for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
            for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
                //Only include deteriorations
                if (crossoverImprovement[i][j] < 0){
                    file12 << crossoverImprovement[i][j] << " ";
                }
            }
        }
        file12.close();
    } else {
        std::cerr << "Failed to open data file: " << "rateOfDeteriorationCrossovers.txt" << std::endl;
    }
    
}


int main(int argc, char **argv) {
    // Check the number of arguments to determine which class to use
    std::string typeOfAnalyzer = "";
    std::string route = "";
    if (argc == 8) {
        // Use metricsAnalyzer class
        std::string problemFile = argv[1];
        int optimum = std::stoi(argv[2]);
        int n_var = std::stoi(argv[3]);
        int mu = std::stoi(argv[4]);
        int entropyZones = std::stoi(argv[5]);
        int R = std::stoi(argv[6]);
        std::string problemName = argv[7];
        bool maxProblem = true;
        //route has to be problemName/n_var/mu 
        route = problemName + "/" + std::to_string(n_var) + "/" + std::to_string(mu) + "/";
        
        int n_firstGen = mu;
        int n_children = mu;

        std::cout << "Using metricsAnalyzer class" << std::endl;
        std::cout << "Problem File: " << problemFile << std::endl;
        std::cout << "Optimum: " << optimum << std::endl;
        std::cout << "n_var: " << n_var << std::endl;
        std::cout << "n_firstGen: " << n_firstGen << std::endl;
        std::cout << "n_children: " << n_children << std::endl;
        std::cout << "Route: " << route << std::endl;
         
        ioh::common::MetricsAnalyzer metricsAnalyzer = ioh::common::MetricsAnalyzer(n_var, n_firstGen, n_children, maxProblem);
        
        metricsAnalyzer = prepareData(metricsAnalyzer, problemFile, n_var, n_firstGen, n_children, optimum, entropyZones);
        createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones, R, maxProblem, route);
        
        typeOfAnalyzer = "populationBased";
    }
    else {
        // Use trayectorialMetrics class
        std::cout << "Using trayectorialMetrics class" << std::endl;
        std::cout << "Number of arguments provided: " << argc - 1 << std::endl;
        
        bool maxProblem = true;

        string path = argv[1];
        int n = stoi(argv[2]);
        int entropySize = stoi(argv[3]);

        route = path + "/";

        outputMetrics(path, n, entropySize);

        typeOfAnalyzer = "trayectorialMetrics";
    }

    std::cout << "Plotting data" << std::endl;
    std::cout << "python3 plotData.py " + route + " " + typeOfAnalyzer << std::endl;
    system(("python3 plotData.py " + route + " " + typeOfAnalyzer).c_str());

    return 0;
}

/*
int main(int argc, char **argv) {
   
    std::string problemFile = argv[1];
    std::string optAndvarName = argv[2]; 
    int mu = std::stoi(argv[4]);
    int entropyZones = std::stoi(argv[5]);
    int R = std::stoi(argv[6]);
    bool maxProblem = true;
    std::cout << "Reading optima and n_vars" << std::endl;
    std::ifstream file(optAndvarName);
    std::cout << optAndvarName << std::endl;
    std::string line;
    std::vector<int> optima;
    std::vector<int> n_vars;
    std::vector<std::string> problemNames;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name;
        int opt;
        int n_var;
        iss >> name >> opt >> n_var;
        problemNames.push_back(name);
        optima.push_back(opt);
        n_vars.push_back(n_var);
    }

    std::cout << "Problem: " << problemNames[problemNumber - 1] << std::endl;

    std::string problemName = problemNames[problemNumber - 1];
    int n_var = n_vars[problemNumber - 1];
    int n_firstGen = mu;
    int n_children = mu;
    int optimum = optima[problemNumber - 1];

    std::cout << "Problem: " << problemName << std::endl;
    std::cout << "Optimum: " << optimum << std::endl;
    std::cout << "n_var: " << n_var << std::endl;
    std::cout << "n_firstGen: " << n_firstGen << std::endl;
    std::cout << "n_children: " << n_children << std::endl;
     
    ioh::common::MetricsAnalyzer metricsAnalyzer = ioh::common::MetricsAnalyzer(n_var,n_firstGen,n_children, maxProblem);
    //route has to be problemName/n_var/mu 
    std::string route = problemName + "/" + std::to_string(n_var) + "/" + std::to_string(mu) + "/";
    metricsAnalyzer = prepareData(metricsAnalyzer, problemFile, n_var, n_firstGen, n_children, optimum, entropyZones);
    createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones, R, maxProblem, route);

    std::cout << "Plotting data" << std::endl;
    system(("python3 plotData.py " + route).c_str());

    return 0;
}*/

/*
std::vector<std::string> find_execution_directories() {
    fs::path base_dir = fs::path("..") / "IOHalgorithm" / "run" / "run";
    std::vector<fs::path> folders;
    std::vector<std::string> result;

    // First, add the base "customea" directory if it exists
    fs::path base_customea = base_dir / "customea";
    if (fs::exists(base_customea)) {
        result.push_back(base_customea.string());
    }

    // Iterate through directory entries to find all folders starting with "customea"
    for (const auto& entry : fs::directory_iterator(base_dir)) {
        if (entry.is_directory() && entry.path().filename().string().find("customea-") == 0) {
            folders.push_back(entry.path());
        }
    }

    if (!folders.empty()) {
        // Sort folders based on the numerical suffix
        std::sort(folders.begin(), folders.end(), [](const fs::path& a, const fs::path& b) {
            auto extract_number = [](const fs::path& path) -> int {
                std::string filename = path.filename().string();
                size_t pos = filename.find('-');
                if (pos != std::string::npos) {
                    return std::stoi(filename.substr(pos + 1));
                }
                return 0;
            };
            return extract_number(a) < extract_number(b);
        });

        // Add all sorted folders to the result vector
        for (const auto& folder : folders) {
            result.push_back(folder.string());
        }
    }

    return result;
}*/