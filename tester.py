import sys
import os
from pathlib import Path


def runAlg():
    # Run the main program with the following parameters

    #Parameters for the problems and suite

    algorithm_name = 'fga'
    suite_name = 'PBO'
    problem_str = '1-25'
    instance_str = '1'
    dimension_str = ['64', '625']
    dir = './'
    runs = 1
    budget = 7500
    seed =  999
    mu_ = [5, 15, 50]

    dim_mu = [(dim, m) for dim in dimension_str for m in mu_]

    #Run
    for dim, mu in dim_mu:
        os.system(f'cd ../IOHalgorithm/run/run/ \n ./main {algorithm_name} {suite_name} \"{problem_str}\" \"{instance_str}\" \"{dim}\" \"{dir}\" {runs} {budget} {seed} {mu} {mu}')

def find_execution_directories():
    # Construct the base directory path
    base_dir = Path("..") / "IOHalgorithm" / "run" / "run"
    folders = []
    result = []
    
    # First, check for the base "customea" directory
    base_customea = base_dir / "fga"
    if base_customea.exists():
        result.append(str(base_customea))
    
    # Find all directories that start with "customea-"
    for entry in base_dir.iterdir():
        if entry.is_dir() and entry.name.startswith("fga-"):
            folders.append(entry)
    
    if folders:
        # Sort folders based on the numerical suffix
        def extract_number(path):
            try:
                # Split on '-' and take the last part as the number
                return int(path.name.split('-')[-1])
            except ValueError:
                return 0
        
        # Sort folders based on their numerical suffix
        folders.sort(key=extract_number)
        
        # Add all sorted folders to the result list
        result.extend(str(folder) for folder in folders)
    
    return result

def get_data_subfolder_paths(base_directory):
    data_folders = [
        "data_f1_OneMax",
        "data_f2_LeadingOnes",
        "data_f3_Linear",
        "data_f4_OneMaxDummy1",
        "data_f5_OneMaxDummy2",
        "data_f6_OneMaxNeutrality",
        "data_f7_OneMaxEpistasis",
        "data_f8_OneMaxRuggedness1",
        "data_f9_OneMaxRuggedness2",
        "data_f10_OneMaxRuggedness3",
        "data_f11_LeadingOnesDummy1",
        "data_f12_LeadingOnesDummy2",
        "data_f13_LeadingOnesNeutrality",
        "data_f14_LeadingOnesEpistasis",
        "data_f15_LeadingOnesRuggedness1",
        "data_f16_LeadingOnesRuggedness2",
        "data_f17_LeadingOnesRuggedness3",
        "data_f18_LABS",
        "data_f19_IsingRing",
        "data_f20_IsingTorus",
        "data_f21_IsingTriangular",
        "data_f22_MIS",
        "data_f23_NQueens",
        "data_f24_ConcatenatedTrap",
        "data_f25_NKLandscapes"
    ]
    return [Path(base_directory) / folder for folder in data_folders]

def runMetrics():
    directories = find_execution_directories()
    baseDir = Path("..") / "IOHalgorithm" / "run" / "run"
    optAndNvarFiles = [baseDir / "optAndNvar64.txt", baseDir / "optAndNvar100.txt", baseDir / "optAndNvar625.txt", baseDir / "optAndNvar2500.txt", baseDir / "optAndNvar5625.txt"]
    muCounter = 0
    for directory in directories:
        data_folders = get_data_subfolder_paths(directory)
        for data_folder in data_folders:
            dir_list = os.listdir(data_folder)
            if 'DIM64' in dir_list[0]:
                optFileToUse = optAndNvarFiles[0]
            elif 'DIM100' in dir_list[0]:
                optFileToUse = optAndNvarFiles[1]
            elif 'DIM625' in dir_list[0]:
                optFileToUse = optAndNvarFiles[2]
            elif 'DIM2500' in dir_list[0]:
                optFileToUse = optAndNvarFiles[3]
            elif 'DIM5625' in dir_list[0]:
                optFileToUse = optAndNvarFiles[4]
            else:
                print("No optAndNVar file found for the data folder: ", data_folder)
                continue
            if muCounter % 3 == 0:
                mu = 5
            elif muCounter % 3 == 1:
                mu = 15
            elif muCounter % 3 == 2:
                mu = 50
            for data_dir in dir_list:
                problemNumber = data_dir[13:]
                problemNumber = problemNumber[:problemNumber.find('_')]
                print(f"ProblemNumber: {problemNumber}")
                data_dir_path = data_folder / data_dir
                print(f"Running metrics for {data_dir_path}")
                os.system(f"./main {data_dir_path} {optFileToUse} {problemNumber} {mu}")
                print(f"Finished running metrics for {data_dir_path}")
        muCounter += 1

def runMetricsForOneInstance(baseDirExtension,  optNvarFile, dataFolderParam, problemNumber, mu, entropyZones, R):
    baseDir = Path("..") / "IOHalgorithm" / "run" / "run"
    baseDataDir = baseDir / baseDirExtension
    dataFolder = Path(baseDataDir) / dataFolderParam
    for data_dir in os.listdir(dataFolder):
        if problemNumber in data_dir:
            dataFolder = Path(dataFolder) / data_dir
    optNvarFile = Path(baseDir) / optNvarFile
    #get the data of the optimal and nvar values by reading optAndNvar file
    optimal = 50
    nvar = 50
    problemName = dataFolderParam
    print(f"Optimal: {optimal}, Nvar: {nvar}")
    print('Running following command: ')
    print(f"./main {dataFolder} {optimal} {nvar} {mu} {entropyZones} {R} {problemName}")
    os.system(f"./main {dataFolder} {optimal} {nvar} {mu} {entropyZones} {R} {problemName}")


import time

if __name__ == "__main__":
    #runAlg()
    start_time = time.time()
    runMetricsForOneInstance('customea', 'optAndNvar50.txt', 'data_f1_OneMax', '1', 5, 8, 8)
    #print("--- %s seconds ---" % (time.time() - start_time))
    #runMetrics()