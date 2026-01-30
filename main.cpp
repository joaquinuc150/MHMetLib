#include "metrics/PopMetrics.hpp"
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
#include <cctype>

namespace fs = std::filesystem;

using HammingDistanceType = ioh::common::HammingDistanceType;
using DiversityCenterType = ioh::common::DiversityCenterType;

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
    metricsAnalyzer.MDSSearchSpaceDivision(entropyZones);

    return metricsAnalyzer;
}

void createFilesForPlots(ioh::common::MetricsAnalyzer& metricsAnalyzer,int n_firstGen, int n_children, int optimum, int entropyZones,int R, bool maxProblem, std::string route,int distanceToCenter, int hdVariant, double qthr, std::vector<Domain> domains = std::vector<Domain>(), std::vector<double> customCenter = std::vector<double>()){
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
    std::cout << route + "ConvGraph_P.txt";
    std::ofstream file1(route + "ConvGraph_P.txt");
    std::vector<double> objectiveValueOverTimeVector = metricsAnalyzer.ConvGraph();
    if (file1.is_open()) {
        for (size_t i = 0; i < objectiveValueOverTimeVector.size(); ++i) {
            file1 << i << " " << objectiveValueOverTimeVector[i] << std::endl;
        }
        file1.close();
    } else {
        std::cerr << "Failed to open data file: " << "ConvGraph_P.txt" << std::endl;
    }
    auto metric_end = std::chrono::high_resolution_clock::now();
    double metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "objectiveValueOverTime " << metric_duration << std::endl;

    std::cout << "objectiveValueBestSolutionOverTime" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot objectiveValueBestSolutionOverTime
    std::ofstream file13(route + "CurrentB_P.txt");
    std::vector<double> objectiveValueBestSolutionOverTimeVector = metricsAnalyzer.CurrentB();
    if (file13.is_open()) {
        for (size_t i = 0; i < objectiveValueBestSolutionOverTimeVector.size(); ++i) {
            file13 << i << " " << objectiveValueBestSolutionOverTimeVector[i] << std::endl;
        }
        file13.close();
    } else {
        std::cerr << "Failed to open data file: " << "CurrentB_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "objectiveValueBestSolutionOverTime " << metric_duration << std::endl;

    std::cout << "improvementInObjectiveValuePerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    std::ofstream fileImp(route + "DistImp_P.txt");
    std::vector<double> improvementVector = metricsAnalyzer.DistImp(maxProblem);
    if (fileImp.is_open()) {
        for (size_t i = 0; i < improvementVector.size(); ++i) {
            fileImp << i << " " << improvementVector[i] << std::endl;
        }
        fileImp.close();
    } else {
        std::cerr << "Failed to open data file: DistImp_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "improvementInObjectiveValuePerGeneration " << metric_duration << std::endl;

    std::cout << "deteriorationInObjectiveValuePerGeneration" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    std::ofstream fileDet(route + "DistDet_P.txt");
    std::vector<double> deteriorationVector = metricsAnalyzer.DistDet(maxProblem);
    if (fileDet.is_open()) {
        for (size_t i = 0; i < deteriorationVector.size(); ++i) {
            fileDet << i << " " << deteriorationVector[i] << std::endl;
        }
        fileDet.close();
    } else {
        std::cerr << "Failed to open data file: DistDet_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "deteriorationInObjectiveValuePerGeneration " << metric_duration << std::endl;

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
    std::ofstream file4(route + "ConvRate_P.txt");
    std::vector<double> convergenceOptimumBasedVector = metricsAnalyzer.ConvRate(optimum, maxProblem);
    if (file4.is_open()) {
        for (size_t i = 0; i < convergenceOptimumBasedVector.size(); ++i) {
            file4 << i << " " << convergenceOptimumBasedVector[i] << std::endl;
        }
        file4.close();
    } else {
        std::cerr << "Failed to open data file: " << "ConvRate_P.txt" << std::endl;
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
    std::cout << "relError" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot relError
    std::ofstream file24(route + "RelError_P.txt");
    std::vector<double> relErrorVector = metricsAnalyzer.RelError(optimum, maxProblem);
    if (file24.is_open()) {
        for (size_t i = 0; i < relErrorVector.size(); ++i) {
            file24 << i << " " << relErrorVector[i] << std::endl;
        }
        file24.close();
    } else {
        std::cerr << "Failed to open data file: " << "RelError_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "relError " << metric_duration << std::endl;

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
    std::vector<double> convergenceStepsVector2 = metricsAnalyzer.calculateConvergenceSteps(maxProblem);
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
    std::ofstream file6(route + "GeoConvRate_P.txt");
    std::vector<double> geometricRateofFitnessChangePerGenerationVector = metricsAnalyzer.GeoConvRate(optimum, n_firstGen, n_children);
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
        std::cerr << "Failed to open data file: " << "GeoConvRate_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "geometricRateofFitnessChangePerGeneration " << metric_duration << std::endl;
    
    std::cout << "e_value" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot e_value
    std::ofstream file15(route + "EValue_P.txt");
    double n_convergence = metricsAnalyzer.calculate_n_convergence();
    double n_quality = metricsAnalyzer.calculate_n_quality(optimum, qthr);
    double eValue = metricsAnalyzer.EValue(n_quality, n_convergence);
    std::cout << "eValue: " << eValue << std::endl;
    if (file15.is_open()) {
        file15 << n_quality << std::endl;
        file15 << n_convergence << std::endl;
        file15 << eValue << std::endl;
        file15.close();
    } else {
        std::cerr << "Failed to open data file: " << "EValue_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "e_value " << metric_duration << std::endl;
    
    std::cout << "hammingDistance" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot hammingDistance based on hdVariant parameter
    HammingDistanceType hdType = static_cast<HammingDistanceType>(hdVariant);
    std::vector<double> hammingVector = metricsAnalyzer.HamDist(hdType, maxProblem);

    std::ofstream fileHD(route + "HamDist_P.txt");
    if (fileHD.is_open()) {
        for (size_t i = 0; i < hammingVector.size(); ++i) {
            fileHD << i << " " << hammingVector[i] << std::endl;
        }
        fileHD.close();
    } else {
        std::cerr << "Failed to open data file: " << "HamDist_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "hammingDistance " << metric_duration << std::endl;

    std::cout << "entropyDiversity" << std::endl;
    metric_start = std::chrono::high_resolution_clock::now();
    //plot entropy diversity for each generation
    std::ofstream file7(route + "EntropyDiv_P.txt");
    std::vector<double> areaScoresVector = metricsAnalyzer.EntropyDiv(entropyZones);
    if (file7.is_open()) {
        for (size_t i = 0; i < areaScoresVector.size(); ++i) {
            file7 << i << " " << areaScoresVector[i] << std::endl;
        }
        file7.close();
    } else {
        std::cerr << "Failed to open data file: " << "EntropyDiv_P.txt" << std::endl;
    }
    metric_end = std::chrono::high_resolution_clock::now();
    metric_duration = std::chrono::duration<double>(metric_end - metric_start).count();
    timing_file << "entropyDiversity " << metric_duration << std::endl;

    std::cout << "Spatial Distance Entropy" << std::endl; //Old Name: distanceToCenter
    metric_start = std::chrono::high_resolution_clock::now();
    //plot diversity distance to center for each generation based on distanceToCenter parameter
    std::ofstream file21(route + "SDistance_P.txt");
    std::vector<std::vector<double>> diversityDistanceToCenterVector;
    if (distanceToCenter == 0) {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenter(R, DiversityCenterType::ZERO_CENTER, std::vector<double>(), domains, maxProblem);
    } else if (distanceToCenter == 1) {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenter(R, DiversityCenterType::BEST_INDIVIDUAL_CENTER, std::vector<double>(), domains, maxProblem);
    } else {
        diversityDistanceToCenterVector = metricsAnalyzer.diversityDistanceToCenter(R, DiversityCenterType::CUSTOM_CENTER, customCenter, domains, maxProblem);
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
    std::ofstream file23(route + "ASID_P.txt");
    std::vector<double> accumSumVector = metricsAnalyzer.ASID(maxProblem);
    if (file23.is_open()) {
        for (size_t i = 0; i < accumSumVector.size(); ++i) {
            file23 << i << " " << accumSumVector[i] << std::endl;
        }
        file23.close();
    } else {
        std::cerr << "Failed to open data file: " << "ASID_P.txt" << std::endl;
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


std::string trim(const std::string& str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }
    
    auto end = str.end();
    do {
        end--;
    } while (end > start && std::isspace(*end));
    
    return std::string(start, end + 1);
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
            config[trim(key)] = trim(value);  // Aplica trim a ambos
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

std::vector<Domain_T> generarDominiosT(const std::string& archivo, int numVars) {
    std::vector<Domain_T> dominios(numVars, {0, 0});
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
    std::string route = "";
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }

    std::string config_file = argv[1];
    auto config = readConfig(config_file);

    string typeOfAnalyzer = config["typeOfAnalyzer"];

    bool isPopulationBased = (typeOfAnalyzer == "populationBased");
    bool isTrajectoryBased = (typeOfAnalyzer == "trajectoryBased");

    if (isPopulationBased) {
        std::cout << "Using metricsAnalyzer class" << std::endl;
        
        std::string problemFile = config["problem_file"];
        int optimum = std::stoi(config["optimum"]);
        int n_var = std::stoi(config["n_var"]);
        int mu = std::stoi(config["mu"]);
        std::string problemName = config["problem_name"];
        // Default entropyZones
        int entropyZones;
        if (config.find("entropy_zones") != config.end() && !config["entropy_zones"].empty()) {
            entropyZones = std::stoi(config["entropy_zones"]);
        } else {
            entropyZones = static_cast<int>(std::ceil(std::log(n_var)));
        }

        // Default R
        int R;
        if (config.find("R") != config.end() && !config["R"].empty()) {
            R = std::stoi(config["R"]);
        } else {
            R = static_cast<int>(std::ceil(std::sqrt(n_var)));
        }

        // Custom center
        std::vector<double> customCenter;
        if (config.find("distance_center") != config.end() && !config["distance_center"].empty()) {
            std::string centerStr = config["distance_center"];
            std::istringstream iss(centerStr);
            std::string val;
            while (std::getline(iss, val, ',')) {
                customCenter.push_back(std::stod(val));
            }
        } else {
            customCenter = std::vector<double>(n_var, 0.0);
        }

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
        std::vector<Domain> domains;
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
            domains = generarDominios(domainFile, n_var);
            std::cout << "Custom domains loaded successfully" << std::endl;
        }
        
        metricsAnalyzer = prepareData(metricsAnalyzer, problemFile, n_var, n_firstGen, n_children, optimum, entropyZones);
        
        if (customDomain){
            createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones,
                            R, maxProblem, route, distanceToCenter, hdVariant, qthr, domains, customCenter);
        }else{
            createFilesForPlots(metricsAnalyzer, n_firstGen, n_children, optimum, entropyZones,
                            R, maxProblem, route, distanceToCenter, hdVariant, qthr);
        }
        typeOfAnalyzer = "populationBased";
    }
    else if (isTrajectoryBased) {
        std::cout << "Using trayectorialMetrics class" << std::endl;
        bool hasOptimum = (config.find("optimum") != config.end() && !config["optimum"].empty());
        double optimum = 0.0;
        std::string problemFile = config["problem_file"];
        std::string problemName = config["problem_name"];
        int n = std::stoi(config["n_var"]);
        int hammingDistanceVariant = std::stoi(config["hd_variant"]);
        HammingDistanceVariantEnum hammingDistanceVariantEnum = static_cast<HammingDistanceVariantEnum>(hammingDistanceVariant);
        int distanceToCenterVariant = std::stoi(config["distance_variant"]);
        DistanceToCenterVariantEnum distanceToCenterVariantEnum = static_cast<DistanceToCenterVariantEnum>(distanceToCenterVariant);
        bool maxProblem = (config["maxProblem"] == "true");
        bool customDomain = (config["customDomain"] == "true");
        int entropyZones;
        if (config.find("entropy_zones") != config.end() && !config["entropy_zones"].empty()) {
            entropyZones = std::stoi(config["entropy_zones"]);
        } else {
            entropyZones = static_cast<int>(std::ceil(std::log(n)));
        }

        // Default R
        int R;
        if (config.find("R") != config.end() && !config["R"].empty()) {
            R = std::stoi(config["R"]);
        } else {
            R = static_cast<int>(std::ceil(std::sqrt(n)));
        }
        double qthr = std::stod(config["qthr"]);

        std::vector<Domain_T> domains = {};
        if (customDomain) {
            std::string domainFile = config["domainFile"];
            domains = generarDominiosT(domainFile, n);
        }

        if (hasOptimum) {
            optimum = std::stod(config["optimum"]);
        }

        if (config.find("path") != config.end() && !config["path"].empty()) {
            route = config["path"];
            if (route.back() != '/') {
                route += '/';
            }
        } else {
            route = problemName + "/" + std::to_string(n);
        }

        outputMetrics(problemFile, route, n, R, qthr, entropyZones, domains, hammingDistanceVariantEnum, maxProblem, hasOptimum, optimum);

        typeOfAnalyzer = "trayectorialBased";
    }
    else {
        std::cerr << "Error: Unknown typeOfAnalyzer, only valid options are 'populationBased' or 'trajectoryBased'" << std::endl;
        return 1;
    }

    std::cout << "Plotting data" << std::endl;
    std::cout << "python3 plotData.py " + route + " " + typeOfAnalyzer << std::endl;
    system(("python3 plotData.py " + route + " " + typeOfAnalyzer).c_str());

    return 0;
}

