from __future__ import annotations

import math
from typing import Dict, Tuple


PRETTY_METRIC_NAMES = {
    "CurrentB_last": r"CurrentB\_last",
    "eta_quality": r"$\eta_{quality}$",
    "eta_conv": r"$\eta_{conv}$",
    "EValue": "EValue",
    "SDistance": "SDistance",
    "HamDist": "HamDist",
    "DistImp": "DistImp",
    "DistDet": "DistDet",
    "EntropyDiv": "EntropyDiv",
    "GeoConvRate": "GeoConvRate",
    "VectorDiversity": "VectorDiversity",
    "AccumulatedIntensify": "AccumulatedIntensify",
    "OperatorApplicationRate": "OperatorApplicationRate",
    "OperatorRateImprovement": "OperatorRateImprovement",
    "OperatorRateWorsening": "OperatorRateWorsening",
    "OperatorApplicationRate_Mutations": r"OperatorApplicationRate\_Mutations",
    "OperatorApplicationRate_Crossovers": r"OperatorApplicationRate\_Crossovers",
    "OperatorRateImprovement_Mutations": r"OperatorRateImprovement\_Mutations",
    "OperatorRateImprovement_Crossovers": r"OperatorRateImprovement\_Crossovers",
    "OperatorRateWorsening_Mutations": r"OperatorRateWorsening\_Mutations",
    "OperatorRateWorsening_Crossovers": r"OperatorRateWorsening\_Crossovers",
    "rateOfImprovementMutations": "rateOfImprovementMutations",
    "rateOfDeteriorationMutations": "rateOfDeteriorationMutations",
    "rateOfImprovementCrossovers": "rateOfImprovementCrossovers",
    "rateOfDeteriorationCrossovers": "rateOfDeteriorationCrossovers",
}


def format_number(x: float, digits: int = 4) -> str:
    """Format a number for LaTeX output."""
    if x != x or math.isnan(x):
        return "--"

    s = f"{x:.{digits}g}"

    if "e+" in s:
        a, b = s.split("e+")
        return rf"{a}\times 10^{{{int(b)}}}"

    if "e-" in s:
        a, b = s.split("e-")
        return rf"{a}\times 10^{{-{int(b)}}}"

    return s


def format_mean_pm_std(mu: float, sd: float, digits: int = 4) -> str:
    """Format mean ± std for LaTeX."""
    return f"{format_number(mu, digits)} $\\pm$ {format_number(sd, digits)}"


def get_pretty_metric_name(metric_name: str) -> str:
    """Return a LaTeX-friendly metric name."""
    if metric_name in PRETTY_METRIC_NAMES:
        return PRETTY_METRIC_NAMES[metric_name]
    return metric_name.replace("_", r"\_")


def build_latex_summary_table(
    aggregated: Dict[str, Tuple[float, float]],
    alg_name: str,
    problem: str,
    mode: str,
    n_execs: int,
    digits: int = 4,
) -> str:
    """Build a LaTeX summary table for a single algorithm."""
    if mode == "T":
        ordered_metrics = [
            "CurrentB_last",
            "eta_quality",
            "eta_conv",
            "EValue",
            "SDistance",
            "HamDist",
            "DistImp",
            "DistDet",
            "EntropyDiv",
            "GeoConvRate",
            "VectorDiversity",
            "AccumulatedIntensify",
            "OperatorApplicationRate",
            "OperatorRateImprovement",
            "OperatorRateWorsening",
            "rateOfImprovementMutations",
            "rateOfDeteriorationMutations",
            "rateOfImprovementCrossovers",
            "rateOfDeteriorationCrossovers",
        ]
    else:
        ordered_metrics = [
            "CurrentB_last",
            "eta_quality",
            "eta_conv",
            "EValue",
            "SDistance",
            "HamDist",
            "DistImp",
            "DistDet",
            "EntropyDiv",
            "GeoConvRate",
            "VectorDiversity",
            "AccumulatedIntensify",
            "OperatorApplicationRate_Mutations",
            "OperatorApplicationRate_Crossovers",
            "OperatorRateImprovement_Mutations",
            "OperatorRateImprovement_Crossovers",
            "OperatorRateWorsening_Mutations",
            "OperatorRateWorsening_Crossovers",
            "rateOfImprovementMutations",
            "rateOfDeteriorationMutations",
            "rateOfImprovementCrossovers",
            "rateOfDeteriorationCrossovers",
        ]

    lines = []
    lines.append(r"\begin{table}[t]")
    lines.append(r"\centering")
    lines.append(r"\small")
    lines.append(r"\begin{tabular}{lc}")
    lines.append(r"\toprule")
    lines.append(r"Metric & Mean $\pm$ Std \\")
    lines.append(r"\midrule")

    for metric in ordered_metrics:
        if metric in aggregated:
            mu, sd = aggregated[metric]
            pretty_name = get_pretty_metric_name(metric)
            lines.append(f"{pretty_name} & {format_mean_pm_std(mu, sd, digits)} \\\\")

    lines.append(r"\bottomrule")
    lines.append(r"\end{tabular}")
    lines.append(
        rf"\caption{{Summary of metrics for {alg_name} on {problem} "
        rf"({n_execs} executions, mode {mode}).}}"
    )
    lines.append(rf"\label{{tab:summary_{alg_name}_{problem}}}")
    lines.append(r"\end{table}")

    return "\n".join(lines)


def build_latex_comparison_table(
    result_a,
    result_b,
    digits: int = 4,
) -> str:
    """Build a LaTeX comparison table between two algorithms."""
    common_keys = sorted(set(result_a.aggregated.keys()) & set(result_b.aggregated.keys()))

    lines = []
    lines.append(r"\begin{table*}[t]")
    lines.append(r"\centering")
    lines.append(r"\small")
    lines.append(r"\begin{tabular}{lcc}")
    lines.append(r"\toprule")
    lines.append(rf"Metric & {result_a.name} & {result_b.name} \\")
    lines.append(r"\midrule")

    for metric in common_keys:
        mu_a, sd_a = result_a.aggregated[metric]
        mu_b, sd_b = result_b.aggregated[metric]
        pretty_name = get_pretty_metric_name(metric)

        lines.append(
            f"{pretty_name} & "
            f"{format_mean_pm_std(mu_a, sd_a, digits)} & "
            f"{format_mean_pm_std(mu_b, sd_b, digits)} \\\\"
        )

    lines.append(r"\bottomrule")
    lines.append(r"\end{tabular}")
    lines.append(
        rf"\caption{{Summary comparison between {result_a.name} and {result_b.name} "
        rf"on {result_a.problem}.}}"
    )
    lines.append(rf"\label{{tab:comparison_{result_a.name}_{result_b.name}_{result_a.problem}}}")
    lines.append(r"\end{table*}")

    return "\n".join(lines)
