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

void createFilesForPlots(ioh::common::MetricsAnalyzer metricsAnalyzer, int n_firstGen, int n_children, int optimum, int entropyZones, int R, bool maxProblem, std::string route = "textData/"){
    fs::path base_path = fs::current_path() / route;
    if (!fs::exists(base_path)) {
        fs::create_directories(base_path);
    }
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto metric_start = start_time;
    std::ofstream timing_file(route + "metric_timings.txt");
    
    std::cout << "objectiveValueOverTime" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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

    metric_start = std::chrono::high_resolution_clock::now();
    std::cout << "relError" << std::endl;
    std::ofstream file24(route + "relError.txt");
    std::vector<double> relErrorVector = metricsAnalyzer.relError(optimum, maxProblem);
    if (file24.is_open()) {
        for (size_t i = 0; i < relErrorVector.size(); ++i) {
            file24 << i << " " << relErrorVector[i] << std::endl;
        }
        file24.close();
    } else {
        std::cerr << "Failed to open data file: " << "relError.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "relError " << metric_duration << std::endl;

    std::cout << "convergenceStepsPerIndividual" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "e_value " << metric_duration << std::endl;
    
    std::cout << "hammingDistanceBestSolutionPerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "hammingDistanceBestSolutionPerGeneration " << metric_duration << std::endl;

    std::cout << "hammingDistanceBestSolutionPerGeneration2" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "hammingDistanceBestSolutionPerGeneration2 " << metric_duration << std::endl;

    std::cout << "hammingDistanceBestSolutionPerGeneration3" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "hammingDistanceBestSolutionPerGeneration3 " << metric_duration << std::endl;

    std::cout << "entropyDiversity" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "diversityDistanceToCenter " << metric_duration << std::endl;

    std::cout << "diversityDistanceToCenter2" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "diversityDistanceToCenter2 " << metric_duration << std::endl;

    std::cout << "accumSum" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
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
    std::ofstream file10(route + "rateOfImprovementCrossovers.txt");
    if (file10.is_open()) {
        for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
            for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
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
    std::ofstream file11(route + "rateOfDeteriorationMutations.txt");
    if (file11.is_open()) {
        for (size_t i = 0; i < mutationImprovement.size(); ++i) {
            for (size_t j = 0; j < mutationImprovement[i].size(); ++j) {
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
    std::ofstream file12(route + "rateOfDeteriorationCrossovers.txt");
    if (file12.is_open()) {
        for (size_t i = 0; i < crossoverImprovement.size(); ++i) {
            for (size_t j = 0; j < crossoverImprovement[i].size(); ++j) {
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
    if (argc == 9) {
        std::string problemFile = argv[1];
        int optimum = std::stoi(argv[2]);
        int n_var = std::stoi(argv[3]);
        int mu = std::stoi(argv[4]);
        int entropyZones = std::stoi(argv[5]);
        int R = std::stoi(argv[6]);
        std::string problemName = argv[7];
        route = argv[8];
        bool maxProblem = true;
        
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
        auto config = readConfig("config.cnf");
        std::string path = config["path"];
        int n = std::stoi(config["n"]);
        int entropySize = std::stoi(config["entropySize"]);
        int hammingDistanceVariant = std::stoi(config["hammingDistanceVariant"]);
        bool maxProblem = (config["maxProblem"] == "true");
        std::string typeOfAnalyzer = config["typeOfAnalyzer"];
        bool customDomain = (config["customDomain"] == "true");

        if (customDomain) {
            std::string domainFile = config["domainFile"];
            std::vector<Domain> domains = generarDominios(domainFile, n);
        }

        route = path + "/";

        outputMetrics(path, n, entropySize, hammingDistanceVariant, maxProblem);
    }

    /*std::cout << "Plotting data" << std::endl;
    std::cout << "python3 plotData.py " + route + " " + typeOfAnalyzer << std::endl;
    system(("python3 plotData.py " + route + " " + typeOfAnalyzer).c_str());
    */
    return 0;
}
