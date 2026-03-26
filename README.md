# MHMetLib

MHMetLib is a library designed for measuring the performance of trajectory and population-based metaheuristic algorithms.

## Requirements

To build and run this program, you need:

- A C++ compiler with C++17 support (e.g., GCC, Clang)
- Python 3.x (for `plotData.py` and data visualization)
- Eigen 3.x (header-only linear algebra library)
- SciPy (for Wilcoxon test)

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/your-repo/MHMetLib.git
   cd MHMetLib
   ```

2. Ensure Eigen headers are available in the `./eigen/` directory.

## Compilation

Compile the program using:

```sh
g++ --std=c++17 -g main.cpp -o main
```

## Usage

The program is configured via config files and supports two analysis modes: **Population-Based** and **Trajectory-Based**.

### Running the Program

```sh
./main <config_file>
```

### Population-Based Analysis

Use a config file starting with `config_p` to analyze population-based metaheuristic algorithms.

**Configuration Parameters**:

| Parameter          | Description                                                          | Example      |
| ------------------ | -------------------------------------------------------------------- | ------------ |
| `typeOfAnalyzer`   | Analysis type: `populationBased` or `trajectoryBased`                 | `populationBased` |
| `problem_file`     | Path to the optimization problem data file                            | `JPRuns/...` |
| `optimum`          | Known optimum value for the problem                                   | `100`        |
| `n_var`            | Number of decision variables                                          | `100`        |
| `mu`               | Population size                                                       | `10`         |
| `problem_name`     | Problem name (used to organize output folders)                        | `OneMax`     |
| `distance_to_center` | Diversity metric type: 0=zero center, 1=best individual, 2=custom  | `0`          |
| `hd_variant`       | Hamming distance variant: 0=from start, 1=consecutive, 2=from best   | `0`          |
| `qthr`             | Quality threshold (normalized 0-1)                                    | `0.95`       |
| `path`             | Output directory for results (optional)                               | `JPResults/` |
| `entropy_zones`    | Number of entropy zones for diversity analysis                        | `4`          |
| `R`                | Resolution parameter for distance-to-center metric                    | `50`         |
| `domainFile`       | Custom domain file (required if `distance_to_center=2`)               | `domains.txt` |

**Configuration File Example** (`config_p_test_default.cnf`):

```
typeOfAnalyzer=populationBased
problem_file=Executions/runsGA/10/100/42/customea/data_f1_OneMax/IOHprofiler_f1_DIM100.dat
optimum=100
n_var=100
mu=10
problem_name=OneMax
distance_to_center=0
hd_variant=0
qthr=0.95
path=GA/GA_OneMax100var_seed42_PopSize10/
entropy_zones=4
R=50
```

**Example Run**:

```sh
./main config_p_test_default.cnf
```

Output will be generated in `JPResults/test_default/` with metrics files and visualizations.

### Trajectory-Based Analysis

Use a config file starting with `config_t` to analyze trajectory-based (single-solution) metaheuristic algorithms.

**Configuration Parameters**:

| Parameter          | Description                                                          | Example           |
| ------------------ | -------------------------------------------------------------------- | ----------------- |
| `typeOfAnalyzer`   | Analysis type: `populationBased` or `trajectoryBased`                 | `trajectoryBased` |
| `problem_file`     | Path to the trajectory data file                                      | `Executions/...`  |
| `optimum`          | Known optimum value (optional)                                        | `100`             |
| `n_var`            | Number of decision variables                                          | `100`             |
| `problem_name`     | Problem name (used to organize output folders)                        | `OneMax`          |
| `distance_variant` | Distance-to-center variant: 0=empty, 1=best, 2=custom                | `1`               |
| `hd_variant`       | Hamming distance variant: 0=from start, 1=consecutive, 2=from best   | `0`               |
| `qthr`             | Quality threshold (normalized 0-1)                                    | `0.9`             |
| `path`             | Output directory for results                                          | `TrajBasedResults/` |
| `R`                | Resolution for distance-to-center metric (auto: √n_var)              | `50`              |
| `entropy_zones`    | Number of entropy zones (auto: ⌈log(n_var)⌉)                        | `4`               |
| `customDomain`     | Enable custom domain definitions                                      | `true`            |
| `domainFile`       | Custom domain file (required if `customDomain=true`)                  | `domains.txt`     |
| `maxProblem`       | Optimization type: `true` for maximization, `false` for minimization | `true`            |

**Configuration File Example** (`config_t_test.cnf`):

```
typeOfAnalyzer=trayectoryBased
problem_file=Executions/runsSA/sa/data_f1_OneMax/IOHprofiler_f1_DIM100.dat
optimum=100
n_var=100
problem_name=OneMax
distance_variant=1
hd_variant=0
qthr=0.9
path=TrajBasedResults/SA_OneMax100
R=50
entropy_zones=4
customDomain=true
domainFile=domains.txt
maxProblem=true
```

**Example Run**:

```sh
./main config_t_test.cnf
```

Output will be generated in `TrajBasedResults/SA_OneMax100/` with metrics files and visualizations.

### Data Visualization

After the main analysis completes, the program automatically calls `plotData.py` to generate visualizations:

```sh
python3 plotData.py <output_path> <typeOfAnalyzer>
```

where typeOfAnalyzer can be trayectoryBased, populationBased or combined.

In the case of plot 'combined' algorithms, an example of usage can be:

```sh
python3 plotData.py combined --pop_dir Examples/GAMetrics/GA_P10_OneMax100v_seed1111
--traj_dir Examples/TrajBasedResults/SA_OneMax100
--raw_data Executions/runsGA/10/100/1111/customea/data_f1_OneMax/IOHprofiler_f1_DIM100.dat
```


**Example Run - Multiple Executions**:

The library can be executed by considering multiple independent executions of a target algorithm.
For this, runMHMetLib.py has a minimal user configuration at the top of the file.
Here, a base configuration file, the path to the IOHProfiler executions (ideally including the used seeds or the logic to generate all paths), the problem name, the library output path, and the final summary path should be provided.
The runMHMetLib.py script, for each provided independent execution, will create a new configuration file (adding the lines to declare the path and problemFile), will execute MHMetLib, 
and, at the end, will produce a summary table containing statistics for all the metrics (average and standard deviation).

If the objective is to compare two algorithms (flag COMPARE_TWO_ALGORITHMS set to true), all the information for the two target algorithms must be provided.
At the end, the library will produce a comparison table in LaTeX format and perform a Wilcoxon paired test to verify statistical differences between the sets of best solutions found by each algorithm (output in COMPARISON_BASE_DIR).

To execute, define all the required paths and variables at the beginning of the runMHMetLib.py script.

```sh
python3.X runMHMetLib.py
```


## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

