#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

enum HammingDistanceVariantEnum { FromZero, BetweenSolutions, FromLocalBest };
enum DistanceToCenterVariantEnum { EmptySolution, BestSolution, Custom };

void createFileOutput(std::string filePath, std::vector<RuntimeInfo>& data, size_t n, int R, int entropySize, HammingDistanceVariantEnum hdVariant, bool maxProblem) {
    std::vector<double> p = {};
    for (int i = 0; i < data.size(); i++) {
        std::string filePathOutput = filePath + "_Run_" + std::to_string(i + 1) + "/";

        std::cerr << "Writing to Run " + std::to_string(i + 1) << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "ConvGraph_T", data[i].getFevals());
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "ConvGraph_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "CurrentB_T", data[i].getFevalsBest());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "CurrentB_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].moveDomainFevals();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "moveDomainFevals: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].getfBest();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "getfBest: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].calculateIntensify();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "calculateIntensify: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_matrix(filePathOutput, "ASID_T", data[i].getSumativeIntensify());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "ASID_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "AccumulatedIntensify_T", data[i].getAccumulatedIntensify());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "AccumulatedIntensify_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "OperatorRate_T", data[i].getOperatorRate());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "OperatorRate_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "EntropyWithSphere", data[i].getEntropyWithSphere());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "EntropyWithSphere: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "SpheresAreas", data[i].getSpheresAreasForIteration(R, p));
        end = std::chrono::high_resolution_clock::now();
        std::cout << "SpheresAreas: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "ConvRate", data[i].getConvRateAbs());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "ConvRate: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "ConvRate_T", data[i].getConvRateOpt());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "ConvRate_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "DistImp_T", data[i].getImprovement());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "DistImp_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "DistDet_T", data[i].getWorsening());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "DistDet_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "RelError_T", data[i].getErrorRate());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "RelError_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "GeoConvRate_T", data[i].getGeomRate());
        end = std::chrono::high_resolution_clock::now();
        std::cout << "GeoConvRate_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "VectorDiversity_T", data[i].getVectorDiversity(5));
        end = std::chrono::high_resolution_clock::now();
        std::cout << "VectorDiversity_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        switch (hdVariant)
        {
            case HammingDistanceVariantEnum::FromZero:
                start = std::chrono::high_resolution_clock::now();
                output_file_vector(filePathOutput, "HamDist_T", data[i].getDistanceHammingFromZero());
                end = std::chrono::high_resolution_clock::now();
                std::cout << "HamDist_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;
                break;
            case HammingDistanceVariantEnum::BetweenSolutions:
                start = std::chrono::high_resolution_clock::now();
                output_file_vector(filePathOutput, "HamDist_T", data[i].getDistanceHamming());
                end = std::chrono::high_resolution_clock::now();
                std::cout << "HamDist_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;
                break;
            case HammingDistanceVariantEnum::FromLocalBest:
                start = std::chrono::high_resolution_clock::now();
                output_file_vector(filePathOutput, "HamDist_T", data[i].getDistanceHammingFromLocalBest());
                end = std::chrono::high_resolution_clock::now();
                std::cout << "HamDist_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;
                break;
        }

        std::cout << "e_value" << std::endl;
        start = std::chrono::high_resolution_clock::now();
        double n_quality = data[i].nQuality();
        double n_convergence = data[i].nConvergence();
        double eValue = data[i].getEValue();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "nQuality, nConvergence, eValue: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;
        vector<double> eValueVector = {n_quality, n_convergence, eValue};
        output_file_vector(filePathOutput, "EValue_T", eValueVector);

        start = std::chrono::high_resolution_clock::now();
        data[i].createXMatrixPerEvaluation();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "createXMatrixPerEvaluation: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].createDissimilarityMatrix();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "createDissimilarityMatrix: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].MDS3();
        end = std::chrono::high_resolution_clock::now();
        std::cout << "MDS3: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        data[i].MDSSearchSpaceDivisionCustomSectors(entropySize);
        end = std::chrono::high_resolution_clock::now();
        std::cout << "MDSSearchSpaceDivisionCustomSectors: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

        start = std::chrono::high_resolution_clock::now();
        output_file_vector(filePathOutput, "EntropyDiv_T", data[i].entropyDiversityCustomAreas(entropySize));
        end = std::chrono::high_resolution_clock::now();
        std::cout << "EntropyDiv_T: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;
    }
}

void outputMetrics(std::string path, int n, int R, double threshold, int entropySize, std::vector<Domain> domains = std::vector<Domain>(), HammingDistanceVariantEnum hdVariant = HammingDistanceVariantEnum::FromZero, bool maxProblem = true) {
    std::cerr << "Reading logger data from " << path << std::endl;
    std::string fileName = path.substr(0, path.find_first_of("/")) + "-" + path.substr(path.find_last_of("/") + 1, path.find_last_of(".") - path.find_last_of("/") - 1);

    auto start = std::chrono::high_resolution_clock::now();
    std::vector<RuntimeInfo> data = readLogger(path, maxProblem, domains, threshold);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "readLogger: " << std::chrono::duration<double>(end - start).count() << " s" << std::endl;

    std::cerr << "Creating file output with base name " << fileName << std::endl;
    createFileOutput(fileName, data, n, R, entropySize, hdVariant, maxProblem);
}
