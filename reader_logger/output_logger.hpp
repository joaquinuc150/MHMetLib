#include <iostream>
#include <fstream>
#include <vector>
#include <string>

void createFileOutput(std::string filePath, std::vector<RuntimeInfo>& data, size_t n, int entropySize) {
    for (int i = 0; i < data.size(); i++) {
        std::string filePathOutput = filePath + "_Run_" + std::to_string(i + 1) + "/";

        std::cerr << "Writing to Run " + std::to_string(i + 1) << std::endl;
        output_file_vector(filePathOutput, "Fevals", data[i].getFevals());
        output_file_vector(filePathOutput, "FevalsBest", data[i].getFevalsBest());
        data[i].moveDomainFevals();
        data[i].getfBest();
        data[i].calculateIntensify(); 
        output_file_vector(filePathOutput, "SumativeIntensify", data[i].getSumativeIntensify());
        output_file_vector(filePathOutput, "AccumulatedIntensify", data[i].getAccumulatedIntensify());
        output_file_vector(filePathOutput, "EntropyWithSphere", data[i].getEntropyWithSphere());
        output_file_vector(filePathOutput, "SpheresAreas", data[i].getSpheresAreasForIteration(n));
        output_file_vector(filePathOutput, "ConvRate", data[i].getConvRate());
        output_file_vector(filePathOutput, "ConvRateOpt", data[i].getConvRateOpt());
        output_file_vector(filePathOutput, "Improvement", data[i].getImprovement());
        output_file_vector(filePathOutput, "ErrorRate", data[i].getErrorRate());
        output_file_vector(filePathOutput, "GeomRate", data[i].getGeomRate());
        output_file_vector(filePathOutput, "VectorDiversity", data[i].getVectorDiversity(5));
        output_file_vector(filePathOutput, "DistanceHammingfromZero", data[i].getDistanceHammingFromZero());
        output_file_vector(filePathOutput, "DistanceHamming", data[i].getDistanceHamming());
        output_file_vector(filePathOutput, "DistanceHammingFromLocalBest", data[i].getDistanceHammingFromLocalBest());
        data[i].createXMatrixPerEvaluation();
        data[i].createDissimilarityMatrix();
        data[i].MDS3();
        data[i].MDSSearchSpaceDivisionCustomSectors(entropySize);
        output_file_vector(filePathOutput, "EntropyDiversity", data[i].entropyDiversityCustomAreas(entropySize));
    }
}

void outputMetrics(std::string path, int n, int entropySize) {
    std::cerr << "Reading logger data from " << path << std::endl;
    std::string fileName = path.substr(0, path.find_first_of("/")) + "-" + path.substr(path.find_last_of("/") + 1, path.find_last_of(".") - path.find_last_of("/") - 1);
    std::vector<RuntimeInfo> data = readLogger(path);
    std::cerr << "Creating file output with base name " << fileName << std::endl;
    createFileOutput(fileName, data, n, entropySize);
}