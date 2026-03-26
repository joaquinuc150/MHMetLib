from __future__ import annotations

import numpy as np

try:
    from scipy.stats import mannwhitneyu, wilcoxon
    SCIPY_AVAILABLE = True
except Exception:
    SCIPY_AVAILABLE = False


def compare_algorithms(result_a: object, result_b: object, metric: str = "CurrentB_last") -> str:
    """Compare two algorithms using Wilcoxon when possible.

    If both samples have the same length, a paired Wilcoxon signed-rank test is used.
    Otherwise, the function falls back to the Mann-Whitney U test.
    """
    lines = []
    lines.append(f"Comparison metric: {metric}")
    lines.append(f"Algorithm A: {result_a.name}")
    lines.append(f"Algorithm B: {result_b.name}")
    lines.append("")

    values_a = np.array(
        [execution.scalar_metrics[metric] for execution in result_a.executions if metric in execution.scalar_metrics],
        dtype=float,
    )
    values_b = np.array(
        [execution.scalar_metrics[metric] for execution in result_b.executions if metric in execution.scalar_metrics],
        dtype=float,
    )

    lines.append(f"n(A) = {len(values_a)}")
    lines.append(f"n(B) = {len(values_b)}")
    lines.append(f"mean(A) = {values_a.mean() if len(values_a) else float('nan')}")
    lines.append(f"mean(B) = {values_b.mean() if len(values_b) else float('nan')}")
    lines.append(f"std(A) = {values_a.std(ddof=1) if len(values_a) > 1 else float('nan')}")
    lines.append(f"std(B) = {values_b.std(ddof=1) if len(values_b) > 1 else float('nan')}")
    lines.append("")

    if not SCIPY_AVAILABLE:
        lines.append("SciPy is not available, so no statistical test was computed.")
        return "\n".join(lines)

    if len(values_a) == 0 or len(values_b) == 0:
        lines.append("Not enough data to compare the algorithms.")
        return "\n".join(lines)

    if len(values_a) == len(values_b):
        try:
            statistic, pvalue = wilcoxon(values_a, values_b, alternative="two-sided")
            lines.append("Test used: Wilcoxon signed-rank (paired)")
            lines.append(f"statistic = {statistic}")
            lines.append(f"p-value = {pvalue}")
            lines.append(f"Different at alpha=0.05? {'YES' if pvalue < 0.05 else 'NO'}")
            return "\n".join(lines)
        except Exception as exc:
            lines.append(f"Wilcoxon failed: {exc}")
            lines.append("Falling back to Mann-Whitney U.")
            lines.append("")

    statistic, pvalue = mannwhitneyu(values_a, values_b, alternative="two-sided")
    lines.append("Test used: Mann-Whitney U (fallback, unpaired)")
    lines.append("Reason: sample sizes differ or the paired test was not applicable.")
    lines.append(f"statistic = {statistic}")
    lines.append(f"p-value = {pvalue}")
    lines.append(f"Different at alpha=0.05? {'YES' if pvalue < 0.05 else 'NO'}")
    return "\n".join(lines)
