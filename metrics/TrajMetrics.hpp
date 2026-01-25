#pragma once

#include <vector>
#include <set>
#include "../metrics/efficiency.hpp"
#include "../utils/prints.hpp"
#include "../utils/utils.hpp"
#include "../utils/SimpleMatrix.h"
#include <eigen/Eigen/Dense>
#include <cfloat>

struct Domain_T {
    double min;
    double max;
};

class RuntimeInfo {
    private:
        std::vector<double> fevals;
        double time;
        std::vector<std::vector<double>> x; // xVectorsPerEvaluation
        std::vector<Domain_T> domains;
        double f_best;
        bool maximize = true;

        // Metrics for search space division
        std::vector<int> areaBySolutions; //Vector de sectores de espacio de busqueda por soluciones
        std::vector<std::vector<int>> searchSpaceAreasBySolutions;
        std::vector<std::vector<std::vector<int>>> searchSpaceAreasBySearchAgent; //Vector de agentes, cada agente tiene un vector por sector de espacio de busqueda
        std::vector<std::vector<int>> searchSpaceAreasByGenerations; //Cada generacion tiene un vector donde la posicion es el agente y el valor es el sector de espacio de busqueda
        Eigen::MatrixXd dissimilarity_matrix;
        Eigen::MatrixXd xMatrix;
        std::vector<bool> intensify_vector;
        double threshold;

    public:
        RuntimeInfo() {
            fevals = {};
            time = {};
            x = {};
            f_best = 0;
        }

        RuntimeInfo(bool maximize) : maximize(maximize) {
            fevals = {};
            time = {};
            x = {};
            f_best = 0;
        }

        RuntimeInfo(bool maximize, std::vector<Domain_T> domains) : maximize(maximize), domains(domains) {
            fevals = {};
            time = {};
            x = {};
            f_best = 0;
        }

        RuntimeInfo(bool maximize, std::vector<Domain_T> domains, double threshold) : maximize(maximize), domains(domains), threshold(threshold) {
            fevals = {};
            time = {};
            x = {};
            f_best = 0;
        }

        RuntimeInfo(std::vector<double> fevals, std::vector<std::vector<double>> x, double f_best)
            : fevals(fevals), x(x), f_best(f_best) {}

        RuntimeInfo(std::vector<double> fevals, double time, std::vector<std::vector<double>> x, double f_best)
            : fevals(fevals), time(time), x(x), f_best(f_best) {}

        void addFevals(double feval) {
            fevals.push_back(feval);
        }

        void setTime(double time) {
            time = time;
        }

        void addX(std::vector<double> x_i) {
            x.push_back(x_i);
        }

        int getFevalsSize() {
            return fevals.size();
        }

        std::vector<double> ConvGraph_T() {
            return fevals;
        }

        std::vector<double> CurrentB_T() {
            std::vector<double> fevals_best;
            for (int i = 0; i < fevals.size(); i++) {
                if (maximize)
                    fevals_best.push_back(*std::max_element(fevals.begin(), fevals.begin() + i));
                else
                    fevals_best.push_back(*std::min_element(fevals.begin(), fevals.begin() + i));
            }
            return fevals_best;
        }

        std::vector<std::vector<double>> getX() {
            return x;
        }

        void moveDomainFevals(){
            fevals = move_domain(fevals);
        }

        double getfBest() {
            f_best = maximize ? *std::max_element(fevals.begin(), fevals.end()) : *std::min_element(fevals.begin(), fevals.end());
            return f_best;
        }

        void print() {
            print_vector("fevals", fevals);
            print_double("time", time);
            for (int i = 0; i < x.size(); i++) {
                print_vector("x_" + std::to_string(i), x[i]);
            }
            print_double("f_best", f_best);
        }
        
        void calculateIntensify() {
            intensify_vector = {};
            for (int i = 1; i < fevals.size(); i++) {
                intensify_vector.push_back(maximize ? fevals[i] >= fevals[i-1] : fevals[i] <= fevals[i-1]);
            }
        }

        std::vector<bool> getIntensify() {
            return intensify_vector;
        }

        bool isEqualsSolutions(std::vector<double> a, std::vector<double> b) {
            if (a.size() != b.size()) return false;
            for (int i = 0; i < a.size(); i++) {
                if (a[i] != b[i]) return false;
            }
            return true;
        }

        std::vector<double> getImprovement() {
            std::vector<double> improvement_vector;
            for (int i = 1; i < fevals.size(); i++) {
                if(maximize && fevals[i] > fevals[i-1]) {
                    improvement_vector.push_back(fevals[i] - fevals[i-1]);
                } else if (!maximize && fevals[i] < fevals[i-1]) {
                    improvement_vector.push_back(fevals[i-1] - fevals[i]);
                }
            }
            return improvement_vector;
        }

        std::vector<double> getWorsening() {
            std::vector<double> worsening_vector;
            for (int i = 1; i < fevals.size(); i++) {
                if(maximize && fevals[i] < fevals[i-1]) {
                    worsening_vector.push_back(fevals[i-1] - fevals[i]);
                } else if (!maximize && fevals[i] > fevals[i-1]) {
                    worsening_vector.push_back(fevals[i] - fevals[i-1]);
                }
            }
            return worsening_vector;
        }

        std::vector<double> getNoChange() {
            std::vector<double> no_change_vector;
            for (int i = 1; i < fevals.size(); i++) {
                if(fevals[i] == fevals[i-1] && isEqualsSolutions(x[i], x[i-1])) {
                    no_change_vector.push_back(0.0);
                }
            }
            return no_change_vector;
        }

        std::vector<double> OperatorRate_T() {
            size_t attempts = fevals.size() - 1;
            size_t improvementAttempts = getImprovement().size();
            size_t worseningAttempts = getWorsening().size();
            size_t noChangeAttempts = getNoChange().size();

            std::vector<double> operator_rate_vector;
            operator_rate_vector.push_back((double) (attempts - noChangeAttempts) / attempts);
            operator_rate_vector.push_back((double) improvementAttempts / attempts);
            operator_rate_vector.push_back((double) worseningAttempts / attempts);

            return operator_rate_vector;
        }

        std::vector<std::vector<double>> ASID_T() {
            std::vector<std::vector<double>> sumative_intensify;
            double sum = 0;
            for (int i = 0; i < intensify_vector.size(); i++) {
                std::vector<double> temp;
                double val = intensify_vector[i] ? 1 : -1;
                temp.push_back(val);
                sum += val;
                temp.push_back(sum);
                sumative_intensify.push_back(temp);
            }
            return sumative_intensify;
        }

        std::vector<double> AccumulatedIntensify_T() {
            std::vector<double> accumulated_intensify;
            double sum = 0;
            for (int i = 0; i < intensify_vector.size(); i++) {
                sum += intensify_vector[i];
                accumulated_intensify.push_back(sum/(i+1));
            }
            return accumulated_intensify;
        }

        std::vector<double> getEntropyWithSphere() {
            int n = x[0].size();
            int sum;
            std::vector<double> entropy_vector;
            std::set<double> radius_solutions;
            for (int i = 0; i < x.size(); i++) {
                sum = 0;
                sum = std::reduce(x[i].begin(), x[i].end(), 0.0);
                radius_solutions.insert(ceil(sum));
                entropy_vector.push_back((double) radius_solutions.size() / n);
            }
            return entropy_vector;
        }

        std::vector<double> getSpheresAreas(int n) {
            std::vector<double> spheres_areas;

            double max_area = 0.0;
            for (const auto& d : domains) {
                double range = d.max - d.min;
                max_area += range * range;
            }
            max_area = sqrt(max_area);

            double min_area = 0.0;

            for (int i = 1; i <= n; i++) {
                spheres_areas.push_back(min_area + i * (max_area - min_area) / n);
            }

            return spheres_areas;
        }

        std::vector<double> getSpheresAreasForIteration(
            int n,
            std::vector<double>& p
        ) {
            n = n == -1 ? x[0].size() : n;
            p = p.empty() ? x[0] : p;
            std::vector<double> spheres_areas = getSpheresAreas(n);
            std::vector<double> spheres_areas_for_iteration;

            for (int i = 0; i < x.size(); i++) {
                double distance = 0.0;
                for (int j = 0; j < x[i].size(); j++) {
                    double range = domains[j].max - domains[j].min;
                    if (range == 0) range = 1;

                    double x_norm = (x[i][j] - domains[j].min) / range;
                    double p_norm = (p[j] - domains[j].min) / range;

                    double diff = x_norm - p_norm;
                    distance += diff * diff;
                }

                distance = sqrt(distance);

                for (int j = 0; j < spheres_areas.size(); j++) {
                    if (distance <= spheres_areas[j]) {
                        spheres_areas_for_iteration.push_back(j);
                        break;
                    }
                }
            }

            return spheres_areas_for_iteration;
        }


        std::vector<double> getConvRate(){
            std::vector<double> conv_rate_vector;
            for (int i = 1; i < fevals.size() - 1; i++) {
                conv_rate_vector.push_back(conv_rate(fevals[i+1], fevals[i], fevals[i-1], maximize));
            }
            return conv_rate_vector;
        }

	    std::vector<double> getConvRateAbs(){
            std::vector<double> conv_rate_vector;
            for (int i = 1; i < fevals.size() - 1; i++) {
                conv_rate_vector.push_back(abs(conv_rate(fevals[i+1], fevals[i], fevals[i-1], maximize)));
            }
            return conv_rate_vector;
        }

        std::vector<double> getConvRateOpt() {
            std::vector<double> conv_rate_vector;
            for (int i = 1; i < fevals.size() - 1; i++) {
                conv_rate_vector.push_back(conv_rate_opt(fevals[i], f_best, fevals[i-1]));
            }
            return conv_rate_vector;
        }

        double getAvgConvRate() {
            double f_average = 0;
            for (const auto& f_i : fevals) {
                f_average += f_i;
            }
            f_average /= fevals.size(); 
            return avg_conv_rate(f_average, f_best);
        }

        std::vector<double> getErrorRate() {
            std::vector<double> error_rate_vector;
            for (int i = 0; i < fevals.size(); i++) {
                error_rate_vector.push_back(error_rate(fevals[i], f_best));
            }
            return error_rate_vector;
        }

        std::vector<double> getErrorRateWithoutBest() {
            std::vector<double> error_rate_vector;
            for (int i = 1; i < fevals.size(); i++) {
                error_rate_vector.push_back(error_rate_without_best(fevals[i], fevals[i-1]));
            }
            return error_rate_vector;
        }

        std::vector<double> getGeomRate() {
            std::vector<double> geom_rate_vector;
            for (int i = 1; i < fevals.size(); i++) {
                geom_rate_vector.push_back(geom_rate(f_best, fevals[i], fevals[0], i));
            }
            return geom_rate_vector;
        }

        std::vector<double> getVectorDiversity(int step = 1) {
            std::vector<double> diversity_vector;
            std::vector<std::vector<double>> sub_solutions;
            std::vector<double> sum_dim(x[0].size(), 0);
            for (int i = 0; i < x.size(); i += step) {
                sub_solutions.push_back(x[i]);
                sum_dim = sum_vector(sum_dim, x[i]);
                diversity_vector.push_back(diversity_optimized(sub_solutions, sum_dim));
            }
            return diversity_vector;
        }

        std::vector<double> getDistanceHamming() {
            std::vector<double> distance_hamming_vector;
            for (int i = 1; i < x.size(); i++) {
                distance_hamming_vector.push_back(distance_hamming_normalized(x[i-1], x[i]));
            }
            return distance_hamming_vector;
        }

        std::vector<double> getDistanceHammingFromZero() {
            std::vector<double> distance_hamming_vector;
            for (int i = 1; i < x.size(); i++) {
                distance_hamming_vector.push_back(distance_hamming_normalized(x[0], x[i]));
            }
            return distance_hamming_vector;
        }

        std::vector<double> getDistanceHammingFromLocalBest() {
            std::vector<double> distance_hamming_vector;
            vector<double> x_localbest = x[0];
            double f_localbest = fevals[0];
            for (int i = 0; i < x.size(); i++) {
                if (f_localbest < fevals[i]) {
                    x_localbest = x[i];
                    f_localbest = fevals[i];
                }
                distance_hamming_vector.push_back(distance_hamming_normalized(x_localbest, x[i]));
            }
            return distance_hamming_vector;
        }

        double getSuccessRate() {
            int success = 0;
            for (int i = 1; i < fevals.size(); i++) {
                if (maximize ? fevals[i] > fevals[i-1] : fevals[i] < fevals[i-1]) {
                    success++;
                }
            }
            return (double) success / fevals.size();
        }

        double nQuality_T() {
            int success = 0;
            for (int i = 1; i < fevals.size(); i++) {
                if (maximize ? fevals[i] > threshold : fevals[i] < threshold) {
                    success++;
                }
            }
            return (double) success / fevals.size();
        }

        double nConvergence_T() {
            return (double) x.size() / nCr(x[0].size(), 2);
        }

        double getEValue_T() {
            return nQuality_T() / nConvergence_T();
        }

        void createXMatrixPerEvaluation(){
            size_t variablesSize = x[0].size();
            size_t n = x.size();
            Eigen::MatrixXd xMatrix(variablesSize, n);
            for (size_t i = 0; i < variablesSize; ++i) {
                xMatrix(i, 0) = x[0][i];
            }
            for (size_t i = 1; i < n; ++i) {
                for (size_t j = 0; j < variablesSize; ++j) {
                    xMatrix(j, i) = x[i][j];
                }
            }
            this->xMatrix = xMatrix;
            //std::cout << xMatrix << std::endl;”‹
        }

        void createDissimilarityMatrix()
        {
            Eigen::MatrixXd dissimilarityMatrix(x.size(), x.size());
            for (size_t i = 0; i < x.size(); ++i)
            {
                for (size_t j = 0; j < x.size(); ++j)
                {
                    dissimilarityMatrix(i, j) = distance_hamming(x[i], x[j]);
                }
            }
            dissimilarity_matrix = dissimilarityMatrix;
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
                areaBySolutions.push_back(sectorIndex);
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
        
	std::vector<double> EntropyDiv_T(int n_areas){
	    std::vector<double> areaScoresVector;
	    const int windowSize = 10;
	    
	    // Procesar en ventanas de 10 soluciones
	    for (size_t windowStart = 0; windowStart < areaBySolutions.size(); windowStart += windowSize) {
	        // Calcular el final de la ventana (puede ser menor que windowSize en la última ventana)
	        size_t windowEnd = std::min(windowStart + windowSize, areaBySolutions.size());
	        
	        // Resetear contadores para cada ventana
	        std::vector<double> areaScores(n_areas, 0);
	        
	        // Contar visitas en la ventana actual
	        for (size_t i = windowStart; i < windowEnd; ++i) {
	            // Skip si está fuera de bounds
	            if (areaBySolutions[i] >= n_areas || areaBySolutions[i] < 0) {
	                continue;
	            }
	            // Incrementar contador del área visitada en esta ventana
	            areaScores[areaBySolutions[i]] += 1;
	        }
	        
	        // Calcular entropía para esta ventana
	        double diversity = 0.0;
	        double totalVisits = windowEnd - windowStart;
	        
	        for (size_t j = 0; j < n_areas; ++j) {
	            if (areaScores[j] > 0) {
	                double probability = areaScores[j] / totalVisits;
	                diversity += probability * log(probability);
	            }
	        }
	        
	        areaScoresVector.push_back(-diversity);  // Entropía de Shannon (negativa)
	    }
	    
	    return areaScoresVector;
	}
};
