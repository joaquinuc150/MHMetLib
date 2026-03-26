from __future__ import annotations

from pathlib import Path
import subprocess
import re
from dataclasses import dataclass
from typing import Dict, Iterable, List, Tuple

from read_metrics import read_execution_summary, aggregate_executions
from latex_utils import build_latex_summary_table, build_latex_comparison_table
from stats_utils import compare_algorithms


MAIN_EXECUTABLE = "./main"
GENERATED_CONFIG_DIR = Path("MultipleExecutions/generated_configs")
COMPARISON_BASE_DIR = Path("Examples/Comparisons")


@dataclass
class ExecutionSummary:
    seed: str
    output_dir: Path
    best_solution: float
    scalar_metrics: Dict[str, float]
    series_metrics: Dict[str, Tuple[float, float]]
    mode: str


@dataclass
class AlgorithmResult:
    name: str
    problem: str
    mode: str
    final_summary_dir: Path
    executions: List[ExecutionSummary]
    aggregated: Dict[str, Tuple[float, float]]


# ============================================================
# COMPACT USER CONFIGURATION
# ============================================================
# Edit only this section in normal usage.
#
# The seed collections can contain 10, 20, 50, or any other number of runs.
# You only need to change the seed lists below.
# ============================================================

COMPARE_TWO_ALGORITHMS = True

PROBLEM_LABEL = "IsingTorus100"
PROBLEM_PATH = "IsingTorus"

# SA example: folder "sa" is mapped to seed 0, while "sa-k" is mapped to seed k.
SA_SEEDS = list(range(10))

# GA example: these are directory names and logical seeds at the same time.
GA_P10_SEEDS = [1111, 123, 2222, 3333, 42, 4444, 456, 5555, 789, 999]


def make_sa_algorithm(
    problem_label: str,
    problem_path: str,
    seeds: Iterable[int],
    enabled: bool = True,
) -> dict:
    """Create a compact SA trajectory-based configuration."""
    seeds = [int(seed) for seed in seeds]
    x_paths = []
    for seed in seeds:
        folder = "sa" if seed == 0 else f"sa-{seed}"
        x_paths.append(
            f"Executions/runsSA/{folder}/data_f20_{problem_path}/IOHprofiler_f20_DIM100.dat"
        )

    return {
        "enabled": enabled,
        "name": "SA",
        "type_mode": "T",
        "problem": problem_label,
        "problem_path": problem_path,
        "base_config": f"MultipleExecutions/conf_SA_{problem_label}_base.cnf",
        "run_output_base_dir": "Examples/TrajBasedResults",
        "final_summary_dir": f"Examples/SA/{problem_label}",
        "use_automatic_path_generation": False,
        "x_file_paths": x_paths,
    }


def make_ga_p_algorithm(
    problem_label: str,
    problem_path: str,
    popsize: int,
    seeds: Iterable[int | str],
    enabled: bool = True,
) -> dict:
    """Create a compact GA population-based configuration."""
    seed_strings = [str(seed) for seed in seeds]

    return {
        "enabled": enabled,
        "name": f"GA_P{popsize}",
        "type_mode": "P",
        "problem": problem_label,
        "problem_path": problem_path,
        "base_config": f"MultipleExecutions/config_p_{problem_label}_P{popsize}.cnf",
        "run_output_base_dir": "Examples/GAMetrics",
        "final_summary_dir": f"Examples/GA_P{popsize}/{problem_label}",
        "use_automatic_path_generation": True,
        "ga_base_dir": f"Executions/runsGA/{popsize}/100",
        "ga_seeds": seed_strings,
        "ga_relative_file": f"customea/data_f20_{problem_path}/IOHprofiler_f20_DIM100.dat",
    }


ALGORITHMS = [
    make_sa_algorithm(PROBLEM_LABEL, PROBLEM_PATH, SA_SEEDS, enabled=True),
    make_ga_p_algorithm(PROBLEM_LABEL, PROBLEM_PATH, popsize=10, seeds=GA_P10_SEEDS, enabled=True),
]


def extract_seed_from_sa_path(x_path: str) -> str:
    """Extract the SA seed from a path such as sa, sa-1, sa-2, and so on."""
    path_obj = Path(x_path)
    for part in path_obj.parts:
        if part == "sa":
            return "0"
        match = re.fullmatch(r"sa-(\d+)", part)
        if match:
            return match.group(1)
    raise ValueError(f"Could not extract the SA seed from path: {x_path}")


def extract_seed_from_ga_path(x_path: str) -> str:
    """Extract the GA seed from a path such as runsGA/<popsize>/100/<seed>/..."""
    parts = Path(x_path).parts
    if "runsGA" in parts:
        idx = parts.index("runsGA")
        if idx + 3 < len(parts):
            return parts[idx + 3]
    raise ValueError(f"Could not extract the GA seed from path: {x_path}")


def extract_seed_from_path(x_path: str, algorithm_name: str) -> str:
    """Dispatch seed parsing according to the algorithm family."""
    if algorithm_name.startswith("SA"):
        return extract_seed_from_sa_path(x_path)
    return extract_seed_from_ga_path(x_path)


def build_x_file_paths_for_algorithm(cfg: dict) -> List[str]:
    """Build the list of input files for one algorithm definition."""
    if not cfg.get("use_automatic_path_generation", False):
        return cfg["x_file_paths"]

    if cfg["name"].startswith("GA"):
        base_dir = Path(cfg["ga_base_dir"])
        relative_file = Path(cfg["ga_relative_file"])
        return [str(base_dir / seed / relative_file) for seed in cfg["ga_seeds"]]

    raise ValueError(f"Unsupported automatic path generation for algorithm {cfg['name']}")


def create_config_file(cfg: dict, x_file_path: str) -> Tuple[Path, Path, str]:
    """Create a configuration file by prepending problem_file and path lines."""
    algorithm = cfg["name"]
    problem = cfg["problem"]
    seed = extract_seed_from_path(x_file_path, algorithm)

    base_config = Path(cfg["base_config"])
    run_output_base_dir = Path(cfg["run_output_base_dir"])

    output_dir = run_output_base_dir / f"{algorithm}_{problem}_Seed{seed}"
    output_dir.mkdir(parents=True, exist_ok=True)

    GENERATED_CONFIG_DIR.mkdir(parents=True, exist_ok=True)
    config_path = GENERATED_CONFIG_DIR / f"conf_{algorithm}_{problem}_Seed{seed}.cnf"

    with open(base_config, "r", encoding="utf-8") as handle:
        base_content = handle.read()

    new_content = (
        f"problem_file={x_file_path}\n"
        f"path={output_dir}\n"
        f"{base_content}"
    )

    with open(config_path, "w", encoding="utf-8") as handle:
        handle.write(new_content)

    return config_path, output_dir, seed


def run_main_with_config(config_path: Path) -> bool:
    """Execute the external binary with the generated configuration file."""
    command = [MAIN_EXECUTABLE, str(config_path)]
    print(f"Running: {' '.join(command)}")
    result = subprocess.run(command, check=False)
    return result.returncode == 0


def write_best_solutions_file(
    executions: List[ExecutionSummary],
    out_path: Path,
    alg_name: str,
    problem: str,
) -> None:
    """Write the best solution found in each execution."""
    with open(out_path, "w", encoding="utf-8") as handle:
        handle.write(f"# Best solutions for {alg_name} on {problem}\n")
        handle.write("# seed\tbest_solution\toutput_dir\n")
        for execution in sorted(executions, key=lambda item: item.seed):
            handle.write(f"{execution.seed}\t{execution.best_solution}\t{execution.output_dir}\n")


def write_execution_details_file(executions: List[ExecutionSummary], out_path: Path) -> None:
    """Write an auxiliary file with all scalar and series summaries per execution."""
    with open(out_path, "w", encoding="utf-8") as handle:
        for execution in sorted(executions, key=lambda item: item.seed):
            handle.write(f"seed={execution.seed}\n")
            handle.write(f"mode={execution.mode}\n")
            handle.write(f"output_dir={execution.output_dir}\n")
            for key, value in sorted(execution.scalar_metrics.items()):
                handle.write(f"  {key}={value}\n")
            for key, (mu, sigma) in sorted(execution.series_metrics.items()):
                handle.write(f"  {key}_mean={mu}, {key}_std={sigma}\n")
            handle.write("\n")


def process_algorithm(cfg: dict) -> AlgorithmResult:
    """Run one algorithm over all configured seeds and build its summary artifacts."""
    alg_name = cfg["name"]
    problem = cfg["problem"]
    mode = cfg["type_mode"].upper()

    base_config = Path(cfg["base_config"])
    final_summary_dir = Path(cfg["final_summary_dir"])
    final_summary_dir.mkdir(parents=True, exist_ok=True)

    if not base_config.exists():
        raise FileNotFoundError(f"Base configuration file does not exist: {base_config}")

    x_paths = build_x_file_paths_for_algorithm(cfg)
    executions: List[ExecutionSummary] = []

    for x_path in x_paths:
        x_file = Path(x_path)
        if not x_file.exists():
            print(f"[WARNING] Input file does not exist, skipping: {x_file}")
            continue

        try:
            config_path, output_dir, seed = create_config_file(cfg, str(x_file))

            ok = run_main_with_config(config_path)
            if not ok:
                print(f"[ERROR] Execution failed for {alg_name}, seed={seed}")
                continue

            summary = read_execution_summary(output_dir, seed, mode)
            executions.append(
                ExecutionSummary(
                    seed=summary.seed,
                    output_dir=summary.output_dir,
                    best_solution=summary.best_solution,
                    scalar_metrics=summary.scalar_metrics,
                    series_metrics=summary.series_metrics,
                    mode=summary.mode,
                )
            )
            print(f"[OK] Summary loaded for {alg_name}, seed={seed}")

        except Exception as exc:
            print(f"[ERROR] Problem while processing {alg_name} with {x_file}: {exc}")

    if not executions:
        raise RuntimeError(f"No valid executions were found for {alg_name}")

    aggregated = aggregate_executions(executions)

    best_solutions_path = final_summary_dir / "best_solutions.txt"
    write_best_solutions_file(executions, best_solutions_path, alg_name, problem)

    details_path = final_summary_dir / "execution_details.txt"
    write_execution_details_file(executions, details_path)

    summary_table = build_latex_summary_table(aggregated, alg_name, problem, mode, len(executions))
    summary_tex_path = final_summary_dir / "summary_metrics.tex"
    with open(summary_tex_path, "w", encoding="utf-8") as handle:
        handle.write(summary_table + "\n")

    print(f"[DONE] {alg_name}: summary artifacts created in {final_summary_dir}")

    return AlgorithmResult(
        name=alg_name,
        problem=problem,
        mode=mode,
        final_summary_dir=final_summary_dir,
        executions=executions,
        aggregated=aggregated,
    )


def main() -> None:
    """Run all enabled algorithms and optionally compare the first two."""
    enabled_cfgs = [cfg for cfg in ALGORITHMS if cfg.get("enabled", False)]
    if not enabled_cfgs:
        raise RuntimeError("No algorithms are enabled.")

    results: List[AlgorithmResult] = []
    for cfg in enabled_cfgs:
        results.append(process_algorithm(cfg))

    if COMPARE_TWO_ALGORITHMS and len(results) >= 2:
        result_a, result_b = results[0], results[1]

        comparison_dir = COMPARISON_BASE_DIR / f"{result_a.name}_vs_{result_b.name}" / result_a.problem
        comparison_dir.mkdir(parents=True, exist_ok=True)

        stats_report = compare_algorithms(result_a, result_b, metric="CurrentB_last")
        stats_path = comparison_dir / "statistical_test.txt"
        with open(stats_path, "w", encoding="utf-8") as handle:
            handle.write(stats_report + "\n")

        comparison_table = build_latex_comparison_table(result_a, result_b)
        comparison_tex_path = comparison_dir / "comparison_summary.tex"
        with open(comparison_tex_path, "w", encoding="utf-8") as handle:
            handle.write(comparison_table + "\n")

        print("\n=== COMPARISON FINISHED ===")
        print(f"Comparison folder: {comparison_dir}")
        print(f"Statistical test:  {stats_path}")
        print(f"LaTeX table:        {comparison_tex_path}")


if __name__ == "__main__":
    main()
