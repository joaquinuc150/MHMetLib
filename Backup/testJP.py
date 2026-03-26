import os
import subprocess
import glob
import time

def find_dat_files(base_path="JPRuns"):
    dat_files = []
    pattern = os.path.join(base_path, "*", "*", "*", "customea", "data_f*", "*.dat")
    
    for dat_file in glob.glob(pattern):
        parts = dat_file.split(os.sep)
        if len(parts) >= 6:
            population_size = parts[1]
            dimension = parts[2] 
            seed = parts[3]
            data_folder = parts[5]
            
            problem_info = extract_problem_info(data_folder, dat_file, int(dimension))
            if problem_info:
                dat_files.append({
                    'file_path': dat_file,
                    'population_size': int(population_size),
                    'dimension': int(dimension),
                    'seed': int(seed),
                    'problem_id': problem_info['problem_id'],
                    'problem_name': problem_info['problem_name'],
                    'optimum': problem_info['optimum'],
                    'n_vars': problem_info['n_vars']
                })
    
    return dat_files

def extract_problem_info(data_folder, dat_file, dimension):
    problem_map = {
        'data_f1_OneMax': {'problem_id': 1, 'problem_name': 'OneMax'},
        'data_f9_OneMaxRuggedness2': {'problem_id': 9, 'problem_name': 'OneMaxRuggedness2'},
        'data_f20_IsingTorus': {'problem_id': 20, 'problem_name': 'IsingTorus'},
        'data_f23_NQueens': {'problem_id': 23, 'problem_name': 'NQueens'}
    }
    
    if data_folder in problem_map:
        info = problem_map[data_folder].copy()
        optimum, n_vars = read_optimum_and_nvars(dimension, info['problem_name'])
        info['optimum'] = optimum
        info['n_vars'] = n_vars
        return info
    return None

def read_metric_timings(route):
    timing_file = route + "timing_summary.txt"
    metric_timings = {}
    
    try:
        with open(timing_file, 'r') as f:
            total_analysis_time = float(f.read().strip())
    except:
        total_analysis_time = 0
    
    try:
        metric_file = route + "metric_timings.txt"
        with open(metric_file, 'r') as f:
            for line in f:
                parts = line.strip().split()
                if len(parts) == 2:
                    metric_name = parts[0]
                    metric_time = float(parts[1])
                    metric_timings[metric_name] = metric_time
    except:
        pass
    
    return total_analysis_time, metric_timings

def read_optimum_and_nvars(dimension, problem_name):
    filename = f"optAndNVar{dimension}.txt"
    try:
        with open(filename, 'r') as f:
            lines = f.read().strip().split('\n')
            for line in lines:
                parts = line.strip().split()
                if len(parts) == 3 and parts[0] == problem_name:
                    optimum = int(parts[1])
                    n_vars = int(parts[2])
                    return optimum, n_vars
        
        return dimension, dimension
    except FileNotFoundError:
        return dimension, dimension
    except Exception as e:
        return dimension, dimension
    filename = f"optAndNVar{dimension}.txt"
    try:
        with open(filename, 'r') as f:
            lines = f.read().strip().split('\n')
            for line in lines:
                parts = line.strip().split()
                if len(parts) == 3 and parts[0] == problem_name:
                    optimum = int(parts[1])
                    n_vars = int(parts[2])
                    return optimum, n_vars
        
        print(f"Warning: {problem_name} not found in {filename}")
        return dimension, dimension
    except FileNotFoundError:
        return dimension, dimension
    except Exception as e:
        return dimension, dimension

def run_analysis(dat_info, timing_file):
    route = f"{dat_info['problem_name']}/{dat_info['n_vars']}/{dat_info['population_size']}/{dat_info['seed']}/"
    
    cmd = [
        "./main",
        dat_info['file_path'],
        str(dat_info['optimum']),
        str(dat_info['n_vars']),
        str(dat_info['population_size']),
        "16",  # entropyZones
        "10",  # R
        dat_info['problem_name'],
        route
    ]
    
    start_time = time.time()
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        elapsed = time.time() - start_time
        
        total_analysis_time, metric_timings = read_metric_timings(route)
        
        timing_file.write(f"{dat_info['problem_name']},{dat_info['population_size']},{dat_info['dimension']},{dat_info['seed']},{elapsed:.3f},{total_analysis_time:.3f}")
        
        all_metrics = [
            'objectiveValueOverTime', 'objectiveValueBestSolutionOverTime', 'changeInObjectiveValuePerGeneration',
            'rateOfChangePerIndividualPerGeneration', 'convergenceOptimumBased', 'classicConvergenceOptimumBased',
	    'relError','convergenceStepsPerIndividual', 'convergenceSteps', 'geometricRateofFitnessChangePerGeneration',
            'e_value', 'hammingDistanceBestSolutionPerGeneration', 'hammingDistanceBestSolutionPerGeneration2',
            'hammingDistanceBestSolutionPerGeneration3', 'entropyDiversity', 'diversityDistanceToCenter',
            'diversityDistanceToCenter2', 'accumSum', 'rateOfImprovementMutations', 'rateOfModificationsPerOperator'
        ]
        
        for metric in all_metrics:
            if metric in metric_timings:
                timing_file.write(f",{metric_timings[metric]:.3f}")
            else:
                timing_file.write(",0")
        
        timing_file.write("\n")
        timing_file.flush()
        
        return True
    except subprocess.CalledProcessError as e:
        elapsed = time.time() - start_time
        timing_file.write(f"{dat_info['problem_name']},{dat_info['population_size']},{dat_info['dimension']},{dat_info['seed']},{elapsed:.3f},0")
        for i in range(20):  # 19 metrics
            timing_file.write(",0")
        timing_file.write(",FAILED\n")
        timing_file.flush()
        print(f"Failed: {dat_info['problem_name']} pop={dat_info['population_size']} dim={dat_info['dimension']} seed={dat_info['seed']} - {elapsed:.1f}s")
        return False
    except Exception as e:
        elapsed = time.time() - start_time
        timing_file.write(f"{dat_info['problem_name']},{dat_info['population_size']},{dat_info['dimension']},{dat_info['seed']},{elapsed:.3f},0")
        for i in range(20):  # 19 metrics
            timing_file.write(",0")
        timing_file.write(",ERROR\n")
        timing_file.flush()
        print(f"Error: {dat_info['problem_name']} pop={dat_info['population_size']} dim={dat_info['dimension']} seed={dat_info['seed']} - {elapsed:.1f}s")
        return False

def main():
    dat_files = find_dat_files()
    
    if not dat_files:
        print("No .dat files found")
        return
    
    print(f"Found {len(dat_files)} .dat files")
    
    successful = 0
    with open("analysis_timing.csv", "w") as timing_file:
        timing_file.write("problem,population,dimension,seed,total_time_seconds,analysis_time_seconds,objectiveValueOverTime,objectiveValueBestSolutionOverTime,changeInObjectiveValuePerGeneration,rateOfChangePerIndividualPerGeneration,convergenceOptimumBased,classicConvergenceOptimumBased,convergenceStepsPerIndividual,convergenceSteps,geometricRateofFitnessChangePerGeneration,e_value,hammingDistanceBestSolutionPerGeneration,hammingDistanceBestSolutionPerGeneration2,hammingDistanceBestSolutionPerGeneration3,entropyDiversity,diversityDistanceToCenter,diversityDistanceToCenter2,accumSum,rateOfImprovementMutations,rateOfModificationsPerOperator,status\n")
        
        for dat_info in dat_files:
            if run_analysis(dat_info, timing_file):
                successful += 1
    
    print(f"Completed: {successful}/{len(dat_files)} successful")

if __name__ == "__main__":
    main()
