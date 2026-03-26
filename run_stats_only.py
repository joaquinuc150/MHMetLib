from pathlib import Path
import numpy as np
from stats_utils import compare_algorithms


def load_best_solutions(file_path):
    values = []
    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            if line.startswith("#"):
                continue
            parts = line.strip().split()
            if len(parts) >= 2:
                values.append(float(parts[1]))
    return np.array(values, dtype=float)


class Exec:
    def __init__(self, value):
        self.scalar_metrics = {"CurrentB_last": value}


class DummyResult:
    def __init__(self, name, values):
        self.name = name
        self.executions = [Exec(v) for v in values]


sa_file = Path("Examples/SA/IsingTorus100/best_solutions.txt")
ga_file = Path("Examples/GA_P10/IsingTorus100/best_solutions.txt")

vals_sa = load_best_solutions(sa_file)
vals_ga = load_best_solutions(ga_file)

sa = DummyResult("SA", vals_sa)
ga = DummyResult("GA_P10", vals_ga)

result = compare_algorithms(sa, ga, metric="CurrentB_last")
print(result)
