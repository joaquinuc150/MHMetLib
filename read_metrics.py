from __future__ import annotations

from pathlib import Path
import re
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

import numpy as np


NUM_RE = re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?")

# Common series-like metrics whose within-run mean and standard deviation are computed.
COMMON_SERIES_METRICS = [
    "SDistance",
    "HamDist",
    "DistImp",
    "DistDet",
    "EntropyDiv",
    "GeoConvRate",
    "VectorDiversity",
    "AccumulatedIntensify",
]

# Additional files frequently used in population-based algorithms.
EXTRA_RATE_METRICS = [
    "rateOfImprovementMutations",
    "rateOfDeteriorationMutations",
    "rateOfImprovementCrossovers",
    "rateOfDeteriorationCrossovers",
]


@dataclass
class MetricsExecutionSummary:
    seed: str
    output_dir: Path
    best_solution: float
    scalar_metrics: Dict[str, float]
    series_metrics: Dict[str, Tuple[float, float]]
    mode: str


def load_series(path: Path, assume_index_value: bool = True) -> np.ndarray:
    """Load numeric values from a text file.

    When assume_index_value is True and a line contains two numbers, the second
    one is treated as the metric value and the first one as an index.
    """
    data: List[float] = []
    with open(path, "r", encoding="utf-8", errors="ignore") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue

            parts = NUM_RE.findall(line)
            if not parts:
                continue

            if len(parts) == 2:
                if assume_index_value:
                    data.append(float(parts[1]))
                else:
                    data.extend(float(x) for x in parts)
            elif len(parts) > 2:
                data.extend(float(x) for x in parts)
            else:
                data.append(float(parts[0]))

    return np.array(data, dtype=float)


def mean_std(values: np.ndarray) -> Tuple[float, float]:
    """Compute sample mean and sample standard deviation."""
    if values.size == 0:
        return float("nan"), float("nan")
    if values.size == 1:
        return float(values.mean()), float("nan")
    return float(values.mean()), float(values.std(ddof=1))


def pick(folder: Path, *candidates: str) -> Optional[Path]:
    """Return the first existing file among the candidate names."""
    for candidate in candidates:
        path = folder / candidate
        if path.is_file():
            return path
    return None


def pick_metric(folder: Path, base_name: str, mode: str) -> Optional[Path]:
    """Resolve the metric filename using the preferred suffix for each mode."""
    mode = mode.upper()
    if mode == "P":
        return pick(folder, f"{base_name}_P.txt", f"{base_name}_T.txt", f"{base_name}.txt")
    return pick(folder, f"{base_name}_T.txt", f"{base_name}_P.txt", f"{base_name}.txt")


def parse_operator_rate_P(path: Path) -> Tuple[float, float, float, float]:
    """Parse population-based OperatorRate values.

    Expected order:
    improvement_mutations, improvement_crossovers,
    worsening_mutations, worsening_crossovers.
    """
    rows: List[Tuple[float, float]] = []
    with open(path, "r", encoding="utf-8", errors="ignore") as handle:
        for line in handle:
            parts = NUM_RE.findall(line)
            if len(parts) >= 2:
                rows.append((float(parts[0]), float(parts[1])))

    if len(rows) >= 2:
        return rows[0][0], rows[0][1], rows[1][0], rows[1][1]

    flat = load_series(path, assume_index_value=False)
    if flat.size >= 4:
        return float(flat[0]), float(flat[1]), float(flat[2]), float(flat[3])

    nan = float("nan")
    return nan, nan, nan, nan


def parse_operator_rate_T(path: Path) -> Tuple[float, float]:
    """Parse trajectory-based OperatorRate values.

    Expected order: improvement_rate, worsening_rate.
    """
    flat = load_series(path, assume_index_value=False)
    if flat.size >= 2:
        return float(flat[0]), float(flat[1])
    if flat.size == 1:
        return float(flat[0]), float("nan")
    return float("nan"), float("nan")


def read_last_currentb(folder: Path, mode: str) -> float:
    """Read the last value from CurrentB."""
    path = pick_metric(folder, "CurrentB", mode)
    if path is None:
        raise FileNotFoundError(f"CurrentB file not found in {folder}")
    data = load_series(path, assume_index_value=True)
    if data.size == 0:
        raise ValueError(f"CurrentB file is empty: {path}")
    return float(data[-1])


def read_evalue_triplet(folder: Path, mode: str) -> Tuple[float, float, float]:
    """Read eta_quality, eta_conv, and EValue from EValue.*.txt."""
    path = pick_metric(folder, "EValue", mode)
    if path is None:
        raise FileNotFoundError(f"EValue file not found in {folder}")
    data = load_series(path, assume_index_value=True)
    if data.size < 3:
        raise ValueError(f"EValue file should contain at least three values: {path}")
    return float(data[0]), float(data[1]), float(data[2])


def read_scalar_metric(folder: Path, base_name: str, mode: str) -> float:
    """Read a scalar metric from disk."""
    path = pick_metric(folder, base_name, mode)
    if path is None:
        raise FileNotFoundError(f"{base_name} file not found in {folder}")
    data = load_series(path, assume_index_value=False)
    if data.size == 0:
        raise ValueError(f"{base_name} file is empty: {path}")
    return float(data[0])


def read_series_metric_mean_std(folder: Path, base_name: str, mode: str) -> Tuple[float, float]:
    """Read a series-like metric and summarize it within a single run."""
    path = pick_metric(folder, base_name, mode)
    if path is None:
        raise FileNotFoundError(f"{base_name} file not found in {folder}")
    data = load_series(path, assume_index_value=True)
    if data.size == 0:
        raise ValueError(f"{base_name} file is empty: {path}")
    return mean_std(data)


def read_execution_summary(folder: Path, seed: str, mode: str) -> MetricsExecutionSummary:
    """Read all metrics produced by a single execution folder."""
    best_solution = read_last_currentb(folder, mode)
    eta_quality, eta_conv, evalue = read_evalue_triplet(folder, mode)

    scalar_metrics: Dict[str, float] = {
        "CurrentB_last": best_solution,
        "eta_quality": eta_quality,
        "eta_conv": eta_conv,
        "EValue": evalue,
    }
    series_metrics: Dict[str, Tuple[float, float]] = {}

    for metric in COMMON_SERIES_METRICS:
        path = pick_metric(folder, metric, mode)
        if path is not None:
            series_metrics[metric] = read_series_metric_mean_std(folder, metric, mode)

    path_op_app = pick_metric(folder, "OperatorApplicationRate", mode)
    path_op_rate = pick_metric(folder, "OperatorRate", mode)

    if path_op_app is not None:
        if mode.upper() == "T":
            scalar_metrics["OperatorApplicationRate"] = read_scalar_metric(folder, "OperatorApplicationRate", mode)
        else:
            values = load_series(path_op_app, assume_index_value=False)
            scalar_metrics["OperatorApplicationRate_Mutations"] = float(values[0]) if values.size >= 1 else float("nan")
            scalar_metrics["OperatorApplicationRate_Crossovers"] = float(values[1]) if values.size >= 2 else float("nan")

    if path_op_rate is not None:
        if mode.upper() == "T":
            improvement_rate, worsening_rate = parse_operator_rate_T(path_op_rate)
            scalar_metrics["OperatorRateImprovement"] = improvement_rate
            scalar_metrics["OperatorRateWorsening"] = worsening_rate
        else:
            improvement_mut, improvement_cross, worsening_mut, worsening_cross = parse_operator_rate_P(path_op_rate)
            scalar_metrics["OperatorRateImprovement_Mutations"] = improvement_mut
            scalar_metrics["OperatorRateImprovement_Crossovers"] = improvement_cross
            scalar_metrics["OperatorRateWorsening_Mutations"] = worsening_mut
            scalar_metrics["OperatorRateWorsening_Crossovers"] = worsening_cross

    for metric in EXTRA_RATE_METRICS:
        path = pick_metric(folder, metric, mode)
        if path is not None:
            series_metrics[metric] = read_series_metric_mean_std(folder, metric, mode)

    return MetricsExecutionSummary(
        seed=seed,
        output_dir=folder,
        best_solution=best_solution,
        scalar_metrics=scalar_metrics,
        series_metrics=series_metrics,
        mode=mode,
    )


def aggregate_executions(executions: List[object]) -> Dict[str, Tuple[float, float]]:
    """Aggregate metrics across executions using mean and sample standard deviation.

    For series-like metrics, the per-run mean is aggregated across runs.
    """
    metric_values: Dict[str, List[float]] = {}

    for execution in executions:
        for key, value in execution.scalar_metrics.items():
            metric_values.setdefault(key, []).append(value)
        for key, (mu, _sigma) in execution.series_metrics.items():
            metric_values.setdefault(key, []).append(mu)

    aggregated: Dict[str, Tuple[float, float]] = {}
    for key, values in metric_values.items():
        aggregated[key] = mean_std(np.array(values, dtype=float))

    return aggregated

