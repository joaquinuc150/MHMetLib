import subprocess
import sys
from itertools import product
import time
import os

def run_experiment(algorithm_name, suite_name, problem_id, instance_id, 
                  dimension, output_dir, runs, budget, seed, population_size):
    full_output_dir = os.path.join("./runs", str(population_size), str(dimension), str(seed))
    os.makedirs(full_output_dir, exist_ok=True)
    
    cmd = [
        "./main",
        algorithm_name,
        suite_name,
        problem_id,
        instance_id,
        str(dimension),
        full_output_dir,
        str(runs),
        str(budget),
        str(seed),
        str(population_size),
        str(population_size)
    ]
    
    start_time = time.time()
    try:
        result = subprocess.run(cmd, check=True, capture_output=True, text=True)
        elapsed = time.time() - start_time
        print(f"seed={seed}, pop={population_size}, dim={dimension} - {elapsed:.2f}s")
        return True
    except subprocess.CalledProcessError as e:
        elapsed = time.time() - start_time
        print(f"Failed: seed={seed}, pop={population_size}, dim={dimension} - {elapsed:.2f}s - {e}")
        return False
    except Exception as e:
        elapsed = time.time() - start_time
        print(f"Error: seed={seed}, pop={population_size}, dim={dimension} - {elapsed:.2f}s - {e}")
        return False

def main():
    algorithm_name = "customea"
    suite_name = "pbo"
    problem_id = "1,9,20,23"
    instance_id = "1"
    output_dir = "./JPRuns"
    runs = 1
    budget = 1000
    
    seeds = [42, 123, 456, 789, 999, 1111, 2222, 3333, 4444, 5555]
    dimensions = [100, 900]
    population_sizes = [10, 20, 50]
    
    print(f"Running {len(seeds) * len(dimensions) * len(population_sizes)} experiments...")
    
    successful = 0
    total = len(seeds) * len(dimensions) * len(population_sizes)
    
    for seed, dim, pop_size in product(seeds, dimensions, population_sizes):
        if run_experiment(algorithm_name, suite_name, problem_id, instance_id,
                         dim, output_dir, runs, budget, seed, pop_size):
            successful += 1
    
    print(f"Completed: {successful}/{total} successful")
    sys.exit(1)

if __name__ == "__main__":
    main()
