#!/usr/bin/env python3
"""
MakeTexTable.py

Generate LaTeX tables from metric text files.

Whitelisted metrics:
Required:
- CurrentB*.txt                     -> last value (best-so-far)
- EValue*.txt                       -> first 3 numbers: eta_quality, eta_conv, EValue
- SDistance*.txt, HamDist*.txt,
  DistImp*.txt, DistDet*.txt         -> mean ± std (sample std, ddof=1)

Operators:
Population-based (mode=P):
- OperatorApplicationRate*.txt       -> (Mutations, Crossovers)  [2 values]
- OperatorRate*.txt                  -> Improvements (Mut,Cross) and Worsenings (Mut,Cross) [2 lines x 2 values]
  (fallback: 4 numbers total: impr_mut, impr_cross, wors_mut, wors_cross)

Trajectory-based (mode=T):
- OperatorApplicationRate*.txt       -> single scalar value
- OperatorRate*.txt                  -> two scalars: (Improvement, Worsening)
  (example line: "0.394394 0.409409")

Allowed extra rate metrics (mean ± std), if present:
- rateOfImprovementMutations(.txt|_P.txt|_T.txt)
- rateOfDeteriorationMutations(.txt|_P.txt|_T.txt)
- rateOfImprovementCrossovers(.txt|_P.txt|_T.txt)
- rateOfDeteriorationCrossovers(.txt|_P.txt|_T.txt)

Everything else in the folder is ignored.

Modes (suffix preference):
- --mode auto|P|T   (default auto)

Formats:
- --format wide|vertical  (default wide)
"""

from __future__ import annotations
import argparse
import os
import re
from dataclasses import dataclass
from typing import Dict, List, Tuple, Optional
import numpy as np

NUM_RE = re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?")

def load_series(path: str, assume_index_value: bool = True) -> np.ndarray:
    """
    Load numeric series from a text file.

    If assume_index_value is True:
        lines with exactly two numbers are treated as (idx, value) -> keep value.
    If False:
        lines with two numbers are treated as two real values -> keep both.
    """
    data: List[float] = []
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
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

def mean_std(x: np.ndarray) -> Tuple[float, float]:
    if x.size == 0:
        return float("nan"), float("nan")
    if x.size == 1:
        return float(x.mean()), float("nan")
    return float(x.mean()), float(x.std(ddof=1))

def fmt(x: float, digits: int = 4) -> str:
    if x != x:  # NaN
        return "--"
    s = f"{x:.{digits}g}"
    if "e+" in s:
        a, b = s.split("e+")
        return rf"{a}\times 10^{{{int(b)}}}"
    if "e-" in s:
        a, b = s.split("e-")
        return rf"{a}\times 10^{{-{int(b)}}}"
    return s

def pm(mu: float, sd: float, digits: int = 4) -> str:
    return f"{fmt(mu, digits)} $\\pm$ {fmt(sd, digits)}"

def pick(input_dir: str, *candidates: str) -> str:
    for c in candidates:
        p = os.path.join(input_dir, c)
        if os.path.isfile(p):
            return p
    return ""

def pick_metric(input_dir: str, base: str, mode: str) -> str:
    mode = (mode or "auto").upper()
    if mode == "P":
        candidates = [f"{base}_P.txt", f"{base}_T.txt", f"{base}.txt"]
    elif mode == "T":
        candidates = [f"{base}_T.txt", f"{base}_P.txt", f"{base}.txt"]
    else:
        candidates = [f"{base}_P.txt", f"{base}_T.txt", f"{base}.txt"]
    return pick(input_dir, *candidates)

def parse_operator_rate_P(path: str) -> Tuple[float, float, float, float]:
    """Population OperatorRate -> (impr_mut, impr_cross, wors_mut, wors_cross)."""
    nan = float("nan")
    if not path:
        return nan, nan, nan, nan

    rows: List[Tuple[float, float]] = []
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            nums = NUM_RE.findall(line)
            if len(nums) >= 2:
                rows.append((float(nums[0]), float(nums[1])))
    if len(rows) >= 2:
        return rows[0][0], rows[0][1], rows[1][0], rows[1][1]

    flat = load_series(path, assume_index_value=False)
    if flat.size >= 4:
        return float(flat[0]), float(flat[1]), float(flat[2]), float(flat[3])
    return nan, nan, nan, nan

def parse_operator_rate_T(path: str) -> Tuple[float, float]:
    """Trajectory OperatorRate -> (improvement, worsening). Accepts 2 numbers anywhere."""
    nan = float("nan")
    if not path:
        return nan, nan
    flat = load_series(path, assume_index_value=False)
    if flat.size >= 2:
        return float(flat[0]), float(flat[1])
    if flat.size == 1:
        return float(flat[0]), nan
    return nan, nan

@dataclass
class MetricsRow:
    label: str
    mode: str  # 'P' or 'T'
    currentb: float
    n_quality: float
    n_conv: float
    evalue: float
    sdist: Tuple[float, float]
    ham: Tuple[float, float]
    distimp: Tuple[float, float]
    distdet: Tuple[float, float]

    # OperatorApplicationRate
    op_app_value: float               # used for T
    op_app_mut: float                 # used for P
    op_app_cross: float               # used for P

    # OperatorRate
    op_rate_improvement: float        # used for T
    op_rate_worsening: float          # used for T
    op_rate_impr_mut: float           # used for P
    op_rate_impr_cross: float         # used for P
    op_rate_wors_mut: float           # used for P
    op_rate_wors_cross: float         # used for P

    other_stats: Dict[str, Tuple[float, float]]

def build_row(input_dir: str, label: str, mode: str = "auto") -> MetricsRow:
    modeU = (mode or "auto").upper()
    if modeU == "AUTO":
        # auto mode = just prefer P then T, but for semantics we keep 'AUTO' until we infer.
        # We'll infer by presence of *_P or *_T core files.
        has_p = any(os.path.isfile(os.path.join(input_dir, f"{b}_P.txt")) for b in ["CurrentB","EValue","SDistance"])
        has_t = any(os.path.isfile(os.path.join(input_dir, f"{b}_T.txt")) for b in ["CurrentB","EValue","SDistance"])
        modeU = "P" if has_p and not has_t else ("T" if has_t and not has_p else "P")

    p_currentb = pick_metric(input_dir, "CurrentB", modeU)
    p_evalue   = pick_metric(input_dir, "EValue", modeU)
    p_sdist    = pick_metric(input_dir, "SDistance", modeU)
    p_ham      = pick_metric(input_dir, "HamDist", modeU)
    p_distimp  = pick_metric(input_dir, "DistImp", modeU)
    p_distdet  = pick_metric(input_dir, "DistDet", modeU)

    p_op_app   = pick_metric(input_dir, "OperatorApplicationRate", modeU)
    p_op_rate  = pick_metric(input_dir, "OperatorRate", modeU)

    missing = [name for name, p in [
        ("CurrentB", p_currentb),
        ("EValue", p_evalue),
        ("SDistance", p_sdist),
        ("HamDist", p_ham),
        ("DistImp", p_distimp),
        ("DistDet", p_distdet),
        ("OperatorApplicationRate", p_op_app),
    ] if not p]
    if missing:
        raise FileNotFoundError(f"Missing required files in {input_dir}: {', '.join(missing)}")

    currentb = float(load_series(p_currentb)[-1])

    e = load_series(p_evalue)
    if e.size < 3:
        raise ValueError(f"EValue file must contain at least 3 numbers: {p_evalue}")
    n_quality, n_conv, evalue = map(float, e[:3])

    sdist = mean_std(load_series(p_sdist))
    ham   = mean_std(load_series(p_ham))
    distimp = mean_std(load_series(p_distimp))
    distdet = mean_std(load_series(p_distdet))

    # Init operator fields
    nan = float("nan")
    op_app_value = nan
    op_app_mut = nan
    op_app_cross = nan
    op_rate_improvement = nan
    op_rate_worsening = nan
    op_rate_impr_mut = nan
    op_rate_impr_cross = nan
    op_rate_wors_mut = nan
    op_rate_wors_cross = nan

    if modeU == "T":
        # OperatorApplicationRate is a single value
        op_app = load_series(p_op_app, assume_index_value=False)
        op_app_value = float(op_app[0]) if op_app.size >= 1 else nan

        # OperatorRate: first=Improvement, second=Worsening
        op_rate_improvement, op_rate_worsening = parse_operator_rate_T(p_op_rate)
    else:
        # Population: OperatorApplicationRate has (mut,cross)
        op_app = load_series(p_op_app, assume_index_value=False)
        op_app_mut = float(op_app[0]) if op_app.size >= 1 else nan
        op_app_cross = float(op_app[1]) if op_app.size >= 2 else nan

        # Population OperatorRate: improvements/worsenings per operator (mut,cross)
        op_rate_impr_mut, op_rate_impr_cross, op_rate_wors_mut, op_rate_wors_cross = parse_operator_rate_P(p_op_rate)

    # Allowed extra metrics (mean ± std)
    ALLOWED_EXTRA_METRICS_BASE = [
        "rateOfImprovementMutations",
        "rateOfDeteriorationMutations",
        "rateOfImprovementCrossovers",
        "rateOfDeteriorationCrossovers",
    ]
    other_stats: Dict[str, Tuple[float, float]] = {}
    for base in ALLOWED_EXTRA_METRICS_BASE:
        p = pick_metric(input_dir, base, modeU)
        if p:
            other_stats[base] = mean_std(load_series(p))

    return MetricsRow(
        label=label,
        mode=modeU,
        currentb=currentb,
        n_quality=n_quality,
        n_conv=n_conv,
        evalue=evalue,
        sdist=sdist,
        ham=ham,
        distimp=distimp,
        distdet=distdet,
        op_app_value=op_app_value,
        op_app_mut=op_app_mut,
        op_app_cross=op_app_cross,
        op_rate_improvement=op_rate_improvement,
        op_rate_worsening=op_rate_worsening,
        op_rate_impr_mut=op_rate_impr_mut,
        op_rate_impr_cross=op_rate_impr_cross,
        op_rate_wors_mut=op_rate_wors_mut,
        op_rate_wors_cross=op_rate_wors_cross,
        other_stats=other_stats,
    )

def to_latex_wide(row: MetricsRow, digits: int = 4) -> str:
    ordered_rates = [
        "rateOfImprovementMutations",
        "rateOfDeteriorationMutations",
        "rateOfImprovementCrossovers",
        "rateOfDeteriorationCrossovers",
    ]
    other_names = [k for k in ordered_rates if k in row.other_stats]

    if row.mode == "T":
        header_cols = [
            "Alg","CurrentB",r"$\eta_{quality}$",r"$\eta_{conv}$","EValue",
            r"$\overline{SDistance}\pm\sigma$",r"$\overline{HamDist}\pm\sigma$",
            r"$\overline{DistImp}\pm\sigma$",r"$\overline{DistDet}\pm\sigma$",
            "OperatorApplicationRate",
            "OperatorRateImprovement",
            "OperatorRateWorsening",
        ] + other_names

        row_cols = [
            row.label,
            fmt(row.currentb, digits),
            fmt(row.n_quality, digits),
            fmt(row.n_conv, digits),
            fmt(row.evalue, digits),
            pm(*row.sdist, digits),
            pm(*row.ham, digits),
            pm(*row.distimp, digits),
            pm(*row.distdet, digits),
            fmt(row.op_app_value, digits),
            fmt(row.op_rate_improvement, digits),
            fmt(row.op_rate_worsening, digits),
        ] + [pm(*row.other_stats[n], digits) for n in other_names]
    else:
        header_cols = [
            "Alg","CurrentB",r"$\eta_{quality}$",r"$\eta_{conv}$","EValue",
            r"$\overline{SDistance}\pm\sigma$",r"$\overline{HamDist}\pm\sigma$",
            r"$\overline{DistImp}\pm\sigma$",r"$\overline{DistDet}\pm\sigma$",
            "OpAppMut","OpAppCross",
            "OpRateImprMut","OpRateImprCross",
            "OpRateWorsMut","OpRateWorsCross",
        ] + other_names

        row_cols = [
            row.label,
            fmt(row.currentb, digits),
            fmt(row.n_quality, digits),
            fmt(row.n_conv, digits),
            fmt(row.evalue, digits),
            pm(*row.sdist, digits),
            pm(*row.ham, digits),
            pm(*row.distimp, digits),
            pm(*row.distdet, digits),
            fmt(row.op_app_mut, digits),
            fmt(row.op_app_cross, digits),
            fmt(row.op_rate_impr_mut, digits),
            fmt(row.op_rate_impr_cross, digits),
            fmt(row.op_rate_wors_mut, digits),
            fmt(row.op_rate_wors_cross, digits),
        ] + [pm(*row.other_stats[n], digits) for n in other_names]

    colspec = "l" + "c" * (len(header_cols) - 1)
    lines = []
    lines += [r"\begin{table}[t]", r"\centering", r"\small",
              r"\begin{tabular}{" + colspec + r"}",
              r"\toprule",
              " & ".join(header_cols) + r" \\",
              r"\midrule",
              " & ".join(row_cols) + r" \\",
              r"\bottomrule",
              r"\end{tabular}",
              r"\caption{Summary of metrics (mean$\pm$std where applicable).}",
              r"\label{tab:metrics-summary}",
              r"\end{table}"]
    return "\n".join(lines)

def to_latex_vertical(row: MetricsRow, digits: int = 4) -> str:
    rows: List[Tuple[str, str]] = []
    rows.append(("Alg", row.label))
    rows.append(("CurrentB (last)", fmt(row.currentb, digits)))
    rows.append((r"$\eta_{quality}$", fmt(row.n_quality, digits)))
    rows.append((r"$\eta_{conv}$", fmt(row.n_conv, digits)))
    rows.append(("EValue", fmt(row.evalue, digits)))
    rows.append((r"SDistance (mean$\pm$std)", pm(*row.sdist, digits)))
    rows.append((r"HamDist (mean$\pm$std)", pm(*row.ham, digits)))
    rows.append((r"DistImp (mean$\pm$std)", pm(*row.distimp, digits)))
    rows.append((r"DistDet (mean$\pm$std)", pm(*row.distdet, digits)))

    if row.mode == "T":
        rows.append(("OperatorApplicationRate", fmt(row.op_app_value, digits)))
        rows.append(("OperatorRateImprovement", fmt(row.op_rate_improvement, digits)))
        rows.append(("OperatorRateWorsening", fmt(row.op_rate_worsening, digits)))
    else:
        rows.append(("OperatorApplicationRate: Mutations", fmt(row.op_app_mut, digits)))
        rows.append(("OperatorApplicationRate: Crossovers", fmt(row.op_app_cross, digits)))
        rows.append(("OperatorRate Improvements: Mutations", fmt(row.op_rate_impr_mut, digits)))
        rows.append(("OperatorRate Improvements: Crossovers", fmt(row.op_rate_impr_cross, digits)))
        rows.append(("OperatorRate Worsenings: Mutations", fmt(row.op_rate_wors_mut, digits)))
        rows.append(("OperatorRate Worsenings: Crossovers", fmt(row.op_rate_wors_cross, digits)))

    ordered_rates = [
        "rateOfImprovementMutations",
        "rateOfDeteriorationMutations",
        "rateOfImprovementCrossovers",
        "rateOfDeteriorationCrossovers",
    ]
    for key in ordered_rates:
        if key in row.other_stats:
            mu, sd = row.other_stats[key]
            rows.append((f"{key} (mean$\\pm$std)", pm(mu, sd, digits)))

    lines = []
    lines += [r"\begin{table}[t]", r"\centering", r"\small",
              r"\begin{tabular}{ll}",
              r"\toprule",
              r"Attribute & Value \\",
              r"\midrule"]
    for a, v in rows:
        lines.append(f"{a} & {v} \\\\")
    lines += [r"\bottomrule",
              r"\end{tabular}",
              rf"\caption{{Metrics for {row.label} (mean$\pm$std where applicable).}}",
              r"\label{tab:metrics}",
              r"\end{table}"]
    return "\n".join(lines)

def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--input_dir", required=True, help="Directory with metric .txt files")
    ap.add_argument("--label", default="ALG", help="Algorithm label")
    ap.add_argument("--output", required=True, help="Output .tex path")
    ap.add_argument("--digits", type=int, default=4)
    ap.add_argument("--mode", choices=["auto", "P", "T"], default="auto",
                    help="Suffix preference / semantics: P(population), T(trajectory), or auto.")
    ap.add_argument("--format", choices=["wide", "vertical"], default="wide",
                    help="Table layout: wide or vertical.")
    args = ap.parse_args()

    row = build_row(args.input_dir, args.label, mode=args.mode)
    tex = to_latex_vertical(row, digits=args.digits) if args.format == "vertical" else to_latex_wide(row, digits=args.digits)

    with open(args.output, "w", encoding="utf-8") as f:
        f.write(tex + "\n")

if __name__ == "__main__":
    main()


