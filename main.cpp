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
#include <chrono>

namespace fs = std::filesystem;

enum class HammingDistanceType {
            FIRST_GEN_BEST,
            PREVIOUS_GEN_BEST,
            GLOBAL_BEST
        };
        
        enum class DiversityCenterType {
            ZERO_CENTER,
            BEST_INDIVIDUAL_CENTER,
            CUSTOM_CENTER
        };

struct Domain {
    double min;
    double max;
};


ioh::common::MetricsAnalyzer prepareData(ioh::common::MetricsAnalyzer metricsAnalyzer, std::string fileName, int n_var, int n_firstGen, int n_children, int optimum, int entropyZones) {
    std::cout << "Reading data from file" << std::endl;
    metricsAnalyzer.readDataFromFile(fileName);
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

void createFilesForPlots(ioh::common::MetricsAnalyzer& metricsAnalyzer,int n_firstGen, int n_children, int optimum, int entropyZones,int R, bool maxProblem, std::string route,int distanceToCenter, int hdVariant, double qthr, std::vector<Domain> domains = nullptr){
    fs::path base_path = fs::current_path() / route;
    if (!fs::exists(base_path)) {
        fs::create_directories(base_path);
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto metric_start = start_time;
    std::ofstream timing_file(route + "metric_timings.txt");
    
    std::cout << "objectiveValueOverTime" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    auto metric_end = std::chrono::high_resolution_clock::now();
    double metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "objectiveValueOverTime " << metric_duration << std::endl;

    std::cout << "objectiveValueBestSolutionOverTime" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "objectiveValueBestSolutionOverTime " << metric_duration << std::endl;

    std::cout << "changeInObjectiveValuePerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "changeInObjectiveValuePerGeneration " << metric_duration << std::endl;

    std::cout << "rateOfChangePerIndividualPerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "rateOfChangePerIndividualPerGeneration " << metric_duration << std::endl;

    std::cout << "convergenceOptimumBased" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "convergenceOptimumBased " << metric_duration << std::endl;

    std::cout << "classicConvergenceOptimumBased" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "classicConvergenceOptimumBased " << metric_duration << std::endl;

    // metric_start = std::chrono::high_resolution_clock::now();
    // std::cout << "relError" << std::endl;
    // //plot relError
    // std::ofstream file24(route + "relError.txt");
    // std::vector<double> relErrorVector = metricsAnalyzer.relError(optimum, maxProblem);
    // if (file24.is_open()) {
    //     for (size_t i = 0; i < relErrorVector.size(); ++i) {
    //         file24 << i << " " << relErrorVector[i] << std::endl;
    //     }
    //     file24.close();
    // } else {
    //     std::cerr << "Failed to open data file: " << "relError.txt" << std::endl;
    // }
    // metric_end = std::chrono::high_resolution_clock::now();
    // metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    // timing_file << "relError " << metric_duration << std::endl;

    std::cout << "convergenceStepsPerIndividual" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "convergenceStepsPerIndividual " << metric_duration << std::endl;

    std::cout << "convergenceSteps" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "convergenceSteps " << metric_duration << std::endl;

    std::cout << "geometricRateofFitnessChangePerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "geometricRateofFitnessChangePerGeneration " << metric_duration << std::endl;
    
    std::cout << "e_value" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot e_value
    std::ofstream file15(route + "e_value.txt");
    double n_convergence = metricsAnalyzer.calculate_n_convergence();
    double n_quality = metricsAnalyzer.calculate_n_quality(optimum, qthr);
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "e_value " << metric_duration << std::endl;
    
    std::cout << "hammingDistance" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot hammingDistance based on hdVariant parameter
    std::vector<double> hammingVector;
    if (hdVariant == 0)
        hammingVector = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration(HammingDistanceType::GLOBAL_BEST);
    else if (hdVariant == 1)
        hammingVector = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration2(HammingDistanceType::FIRST_GEN_BEST);
    else
        hammingVector = metricsAnalyzer.hammingDistanceBestSolutionPerGeneration3(HammingDistanceType::PREVIOUS_GEN_BEST);

    std::ofstream fileHD(route + "hammingDistance.txt");
    if (fileHD.is_open()) {
        for (size_t i = 0; i < hammingVector.size(); ++i) {
            fileHD << i << " " << hammingVector[i] << std::endl;
        }
        fileHD.close();
    } else {
        std::cerr << "Failed to open data file: " << "hammingDistance.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "hammingDistance " << metric_duration << std::endl;

    std::cout << "entropyDiversity" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "entropyDiversity " << metric_duration << std::endl;

    std::cout << "diversityDistanceToCenter" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot diversity distance to center for each generation based on distanceToCenter parameter
    std::ofstream file21(route + "diversityDistanceToCenter.txt");
    std::vector<std::vector<double>> diversityDistanceToCenterVector;
    if (distanceToCenter == 0) {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenterZeroCenter(R);
    } else if (distanceToCenter == 1) {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenterBestIndividual(R, DiversityCenterType::BEST_INDIVIDUAL_CENTER);
    } else {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenterCustom(R, DiversityCenterType::CUSTOM_CENTER, variableRanges = domains);
    }
    
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "diversityDistanceToCenter " << metric_duration << std::endl;

    std::cout << "accumSum" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "accumSum " << metric_duration << std::endl;

    std::cout << "rateOfImprovementMutations" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "rateOfImprovementMutations " << metric_duration << std::endl;

    std::cout << "rateOfModificationsPerOperator" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "rateOfModificationsPerOperator " << metric_duration << std::endl;
    
    timing_file.close();
    
    auto total_end = std::chrono::high_resolution_clock::now();
    double total_duration = std::chrono::duration<double>(total_end - start_time).count();
    
    std::ofstream summary_file(route + "timing_summary.txt");
    summary_file << total_duration << std::endl;
    summary_file.close();
}


std::map<std::string, std::string> readConfig(const std::string& filename) {
    std::map<std::string, std::string> config;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if(line.empty() || line[0] == '#') continue;
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value)) {
            config[key] = value;
        }
    }
    return config;
}

std::vector<Domain> generarDominios(const std::string& archivo, int numVars) {
    std::vector<Domain> dominios(numVars, {0, 0});
    std::ifstream file(archivo);
    std::string linea;
    bool todos = false;

    while (std::getline(file, linea)) {
        std::istringstream iss(linea);
        std::string nombre;
        double minimo, maximo;
        if (iss >> nombre >> minimo >> maximo) {
            if (nombre == "all") {
                for (int i = 0; i < numVars; ++i) {
                    dominios[i] = {minimo, maximo};
                }
                todos = true;
            } else if (!todos && nombre[0] == 'x') {
                int idx = std::stoi(nombre.substr(1)) - 1;
                if (idx >= 0 && idx < numVars) {
                    dominios[idx] = {minimo, maximo};
                }
            }
        }
    }
    return dominios;
}

int main(int argc, char **argv) {
    std::string typeOfAnalyzer = "";
    std::string route = "";
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    auto config = readConfig(config_file);

    bool isPopulationBased = (config_file.find("config_p") != std::string::npos);
    bool isTrajectoryBased = (config_file.find("config_t") != std::string::npos);

    if (isPopulationBased) {
        std::cout << "Using metricsAnalyzer class" << std::endl;
        
        std::string problemFile = config["problem_file"];
        int optimum = std::stoi(config["optimum"]);
        int n_var = std::stoi(config["n_var"]);
        int mu = std::stoi(config["mu"]);
        int entropyZones = std::stoi(config["entropy_zones"]);
        int R = std::stoi(config["R"]);
        std::string problemName = config["problem_name"];
        int distanceToCenter = std::stoi(config["distance_to_center"]);
        int hdVariant = std::stoi(config["hd_variant"]);
        double qthr = std::stod(config["qthr"]);

        bool maxProblem = true;

        // Route construction: use config path if provided, otherwise construct from problem parameters
        if (config.find("path") != config.end() && !config["path"].empty()) {
            route = config["path"];
            if (route.back() != '/') {
                route += '/';
            }
        } else {
            route = problemName + "/" + std::to_string(n_var) + "/" + std::to_string(mu) + "/";
        }
        
        int n_firstGen = mu;
        int n_children = mu;

        ioh::common::MetricsAnalyzer metricsAnalyzer(n_var, n_firstGen, n_children, maxProblem);
        
        bool customDomain = false;
        if (config.find("distanceToCenter") != config.end()) {
            customDomain = (config["distanceToCenter"] == "2");
        }
        
        if (customDomain) {
            if (config.find("domainFile") == config.end() || config["domainFile"].empty()) {
                std::cerr << "Error: customDomain is true but domainFile is not specified in config" << std::endl;
                return 1;
            }
            std::string domainFile = config["domainFile"];
            std::cout << "Loading custom domains from: " << domainFile << std::endl;
            std::vector<Domain> domains = generarDominios(domainFile, n_var);
            std::cout << "Custom domains loaded successfully" << std::endl;
        }
        
        metricsAnalyzer = prepareData(metricsAnalyzer, problemFile, n_var, n_firstGen, n_children, optimum, entropyZones);
        
        if (customDomain){
            createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones,
                            R, maxProblem, route, distanceToCenter, hdVariant, qthr, domains);
        }else{
            createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones,
                            R, maxProblem, route, distanceToCenter, hdVariant, qthr);
        }
        typeOfAnalyzer = "populationBased";
    }
    else if (isTrajectoryBased) {
        std::cout << "Using trayectorialMetrics class" << std::endl;
        
        bool maxProblem = true;
        
        std::string path = config["path"];
        int n = std::stoi(config["n"]);
        int entropySize = std::stoi(config["entropySize"]);
        int hammingDistanceVariant = std::stoi(config["hammingDistanceVariant"]);

        route = path + "/";

        outputMetrics(path, n, entropySize, hammingDistanceVariant, maxProblem);

        typeOfAnalyzer = "trayectorialMetrics";
    }
    else {
        std::cerr << "Error: Unknown config type. Use config_p.cnf or config_t.cnf" << std::endl;
        return 1;
    }

    std::cout << "Plotting data" << std::endl;
    std::cout << "python3 plotData.py " + route + " " + typeOfAnalyzer << std::endl;
    system(("python3 plotData.py " + route + " " + typeOfAnalyzer).c_str());

    return 0;
}

