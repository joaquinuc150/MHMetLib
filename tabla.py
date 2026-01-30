#!/usr/bin/env python3
"""
make_latex_table.py (clean & deterministic)

Generates a LaTeX table for ONE algorithm (e.g., GA).

Included metrics ONLY:
- CurrentB            -> last value
- EValue              -> eta_quality, eta_conv, EValue (first 3 numbers)
- SDistance           -> mean ± std
- HamDist             -> mean ± std
- DistImp             -> mean ± std
- DistDet             -> mean ± std
- OperatorApplicationRate -> Mutations, Crossovers
- rateOfImprovement / rateOfDeterioration (Mutations/Crossovers) -> mean ± std

All other .txt files are ignored.
"""

import argparse
import os
import re
from typing import Dict, Tuple, List
import numpy as np

NUM_RE = re.compile(r"[-+]?\d*\.?\d+(?:[eE][-+]?\d+)?")

def load_series(path: str) -> np.ndarray:
    data: List[float] = []
    with open(path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            nums = NUM_RE.findall(line)
            if not nums:
                continue
            if len(nums) == 2:
                data.append(float(nums[1]))
            else:
                data.extend(float(x) for x in nums)
    return np.asarray(data, dtype=float)

def mean_std(x: np.ndarray) -> Tuple[float, float]:
    if x.size == 0:
        return float("nan"), float("nan")
    if x.size == 1:
        return float(x.mean()), float("nan")
    return float(x.mean()), float(x.std(ddof=1))

def fmt(x: float, d: int = 4) -> str:
    if x != x:
        return "--"
    s = f"{x:.{d}g}"
    if "e+" in s:
        a, b = s.split("e+")
        return rf"{a}\\times 10^{{{int(b)}}}"
    if "e-" in s:
        a, b = s.split("e-")
        return rf"{a}\\times 10^{{-{int(b)}}}"
    return s

def pm(mu: float, sd: float, d: int = 4) -> str:
    return f"{fmt(mu,d)} $\\pm$ {fmt(sd,d)}"

def pick(input_dir: str, *names: str) -> str:
    for n in names:
        p = os.path.join(input_dir, n)
        if os.path.isfile(p):
            return p
    return ""

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--input_dir", required=True)
    ap.add_argument("--label", default="GA")
    ap.add_argument("--output", required=True)
    ap.add_argument("--digits", type=int, default=4)
    args = ap.parse_args()

    d = args.input_dir

    # Required files
    currentb = pick(d, "CurrentB_P.txt", "CurrentB_T.txt", "CurrentB.txt")
    evalue   = pick(d, "EValue_P.txt", "EValue_T.txt", "EValue.txt")
    sdist    = pick(d, "SDistance_P.txt", "SDistance_T.txt", "SDistance.txt")
    ham      = pick(d, "HamDist_P.txt", "HamDist_T.txt", "HamDist.txt")
    distimp  = pick(d, "DistImp_P.txt", "DistImp_T.txt", "DistImp.txt")
    distdet  = pick(d, "DistDet_P.txt", "DistDet_T.txt", "DistDet.txt")
    oprate   = pick(d, "OperatorApplicationRate_P.txt", "OperatorApplicationRate_T.txt", "OperatorApplicationRate.txt")

    for name, p in [("CurrentB",currentb),("EValue",evalue),("SDistance",sdist),
                    ("HamDist",ham),("DistImp",distimp),("DistDet",distdet),
                    ("OperatorApplicationRate",oprate)]:
        if not p:
            raise FileNotFoundError(f"Missing {name} file in {d}")

    cb = load_series(currentb)[-1]
    ev = load_series(evalue)
    eta_q, eta_c, E = ev[:3]

    s_mu, s_sd = mean_std(load_series(sdist))
    h_mu, h_sd = mean_std(load_series(ham))
    di_mu, di_sd = mean_std(load_series(distimp))
    dd_mu, dd_sd = mean_std(load_series(distdet))

    op = load_series(oprate)
    mut, cross = op[:2]

    RATE_FILES = [
        "rateOfImprovementMutations.txt",
        "rateOfDeteriorationMutations.txt",
        "rateOfImprovementCrossovers.txt",
        "rateOfDeteriorationCrossovers.txt",
    ]

    rate_stats: Dict[str, Tuple[float,float]] = {}
    for f in RATE_FILES:
        p = os.path.join(d, f)
        if os.path.isfile(p):
            rate_stats[os.path.splitext(f)[0]] = mean_std(load_series(p))

    rows = [
        ("Alg", args.label),
        ("CurrentB (last)", fmt(cb,args.digits)),
        (r"$\\eta_{quality}$", fmt(eta_q,args.digits)),
        (r"$\\eta_{conv}$", fmt(eta_c,args.digits)),
        ("EValue", fmt(E,args.digits)),
        ("SDistance (mean$\\pm$std)", pm(s_mu,s_sd,args.digits)),
        ("HamDist (mean$\\pm$std)", pm(h_mu,h_sd,args.digits)),
        ("DistImp (mean$\\pm$std)", pm(di_mu,di_sd,args.digits)),
        ("DistDet (mean$\\pm$std)", pm(dd_mu,dd_sd,args.digits)),
        ("OperatorApplicationRate: Mutations", fmt(mut,args.digits)),
        ("OperatorApplicationRate: Crossovers", fmt(cross,args.digits)),
    ]

    for k in [
        "rateOfImprovementMutations",
        "rateOfDeteriorationMutations",
        "rateOfImprovementCrossovers",
        "rateOfDeteriorationCrossovers",
    ]:
        if k in rate_stats:
            mu, sd = rate_stats[k]
            rows.append((f"{k} (mean$\\pm$std)", pm(mu,sd,args.digits)))

    lines = [
        r"\\begin{table}[t]",
        r"\\centering",
        r"\\small",
        r"\\begin{tabular}{ll}",
        r"\\toprule",
        r"Attribute & Value \\",
        r"\\midrule",
    ]

    for a,v in rows:
        lines.append(f"{a} & {v} \\")

    lines += [
        r"\\bottomrule",
        r"\\end{tabular}",
        rf"\\caption{{Metrics for {args.label} (mean$\\pm$std where applicable).}}",
        r"\\label{tab:metrics-ga}",
        r"\\end{table}",
    ]

    with open(args.output, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))

if __name__ == "__main__":
    main()

