# MHMetLib

MHMetLib is a library designed for measure the performance of trajectory and population-based metaheuristic algorithms.

## Requirements

To build and run this program, you need:

- A C++ compiler (e.g., GCC, Clang)
- Python 3.x (for `plotData.py` and other Python utilities, if applicable)
- Eigen 3.x (for population-based)

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/your-repo/MHMetLib.git
## Compilation

    g++ -I /eigen/Eigen main.cpp -o main

## Usage

1. Population-Based Analysis (metricsAnalyzer)
Run with 7 arguments:
    ```
    ./analyzer <problemFile> <optimum> <n_var> <mu> <entropyZones> <R> <problemName>
| Argument       | Description                                     |
| -------------- | ----------------------------------------------- |
| `problemFile`  | Path to the problem input file                  |
| `optimum`      | Known optimum value for the problem             |
| `n_var`        | Number of decision variables                    |
| `mu`           | Population size (also used for offspring count) |
| `entropyZones` | Number of entropy zones to analyze              |
| `R`            | Number of repetitions for plotting              |
| `problemName`  | Name of the problem (used to build folder path) |

Example:

    ./analyzer problems/problem1.txt 100 30 50 10 20 Knapsack

This will generate an internal route like: Knapsack/30/50/
And will use the metricsAnalyzer class to process the data.

2. Trajectory-Based Analysis (trayectorialMetrics)
Run with 3 arguments:
    ```
    ./analyzer <path> <n> <entropySize>
| Argument      | Description                  |
| ------------- | ---------------------------- |
| `path`        | Path to the trajectory data  |
| `n`           | Number of decision variables |
| `entropySize` | Size of the entropy window   |

Example:

    ./analyzer trajectories/experiment1 30 10
This will trigger the trayectorialMetrics class and generate metrics accordingly.

