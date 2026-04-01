from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


plt.rcParams.update({
    "figure.dpi": 120,
    "savefig.dpi": 300,
    "font.size": 12,
    "axes.labelsize": 12,
    "xtick.labelsize": 11,
    "ytick.labelsize": 11,
    "legend.fontsize": 11,
    "lines.linewidth": 1.2,
})

DEFAULT_FIGSIZE = (10, 6)


def new_fig_ax(figsize=DEFAULT_FIGSIZE):
    fig, ax = plt.subplots(figsize=figsize, constrained_layout=True)
    ax.grid(True, linestyle="--", alpha=0.7)
    return fig, ax


def save_close(fig, outpath: Path) -> None:
    outpath.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(outpath, dpi=300, bbox_inches="tight", pad_inches=0.15)
    plt.close(fig)


def load_txt(path: Path) -> np.ndarray:
    return np.loadtxt(path)


def sanitize_values(values: np.ndarray, fill_value: float = 1.0) -> np.ndarray:
    """
    Replace every non-finite value with a safe constant.

    This fixes GeoConvRate issues where the source file may contain NaN, -NaN,
    +inf, or -inf due to zero denominators.
    """
    values = np.asarray(values, dtype=float)
    bad = ~np.isfinite(values)
    if np.any(bad):
        values = values.copy()
        values[bad] = fill_value
    return values


def get_series_values(data: np.ndarray, prefer_second_col: bool = True, fill_value: float = 1.0) -> np.ndarray:
    data = np.asarray(data)

    if data.ndim == 1:
        values = data.astype(float)
    elif data.ndim == 2 and data.shape[1] >= 2 and prefer_second_col:
        values = data[:, 1].astype(float)
    elif data.ndim == 2 and data.shape[1] == 1:
        values = data[:, 0].astype(float)
    else:
        values = data.astype(float).ravel()

    return sanitize_values(values, fill_value=fill_value)


def rolling_mean(values: np.ndarray, window: int, fill_value: float = 1.0) -> np.ndarray:
    values = sanitize_values(np.asarray(values, dtype=float), fill_value=fill_value)

    if window <= 1 or len(values) == 0:
        return values.copy()

    window = min(window, len(values))
    kernel = np.ones(window, dtype=float) / window
    padded = np.pad(values, (window // 2, window - 1 - window // 2), mode="edge")
    smoothed = np.convolve(padded, kernel, mode="valid")
    return sanitize_values(smoothed, fill_value=fill_value)


def aggregate_segment(segment: np.ndarray, strategy: str, fill_value: float = 1.0) -> float:
    if len(segment) == 0:
        return fill_value

    segment = sanitize_values(np.asarray(segment, dtype=float), fill_value=fill_value)
    segment = segment[np.isfinite(segment)]

    if len(segment) == 0:
        return fill_value

    if strategy == "max":
        return float(np.max(segment))
    if strategy == "min":
        return float(np.min(segment))
    if strategy == "last":
        return float(segment[-1])
    return float(np.mean(segment))


def resample_sa_to_generations(
    sa_values: np.ndarray,
    num_generations: int,
    strategy: str,
    fill_value: float = 1.0,
) -> np.ndarray:
    sa_values = sanitize_values(np.asarray(sa_values, dtype=float), fill_value=fill_value)

    if num_generations <= 0:
        raise ValueError("num_generations must be positive")

    if len(sa_values) == 0:
        return np.array([], dtype=float)

    edges = np.linspace(0, len(sa_values), num_generations + 1)
    resampled = []

    for i in range(num_generations):
        start = int(np.floor(edges[i]))
        end = int(np.floor(edges[i + 1]))

        if end <= start:
            end = min(start + 1, len(sa_values))

        segment = sa_values[start:end]
        if len(segment) == 0:
            idx = min(start, len(sa_values) - 1)
            segment = sa_values[idx:idx + 1]

        resampled.append(aggregate_segment(segment, strategy, fill_value=fill_value))

    return sanitize_values(np.asarray(resampled, dtype=float), fill_value=fill_value)


def get_resample_strategy(metric_name: str, minimize: bool = False) -> str:
    best_like = {"ConvGraph", "CurrentB"}
    if metric_name in best_like:
        return "min" if minimize else "max"
    return "mean"


def get_fill_value(metric_name: str) -> float:
    """
    Choose the replacement value for invalid numeric entries.

    GeoConvRate specifically requires 1.0.
    Other metrics also use 1.0 as a safe default to avoid broken curves.
    """
    return 1.0


def plot_combined_line_generation_axis(
    pop_dir: Path,
    traj_dir: Path,
    output_dir: Path,
    pop_file: str,
    traj_file: str,
    out_name: str,
    metric_name: str,
    ylabel: str,
    smooth_window: int = 1,
    minimize: bool = False,
) -> None:
    pop_path = pop_dir / pop_file
    traj_path = traj_dir / traj_file

    if not pop_path.exists() or not traj_path.exists():
        print(f"Warning: Missing file for {metric_name}")
        return

    fill_value = get_fill_value(metric_name)

    ga_data = load_txt(pop_path)
    sa_data = load_txt(traj_path)

    ga_values = get_series_values(ga_data, prefer_second_col=True, fill_value=fill_value)
    sa_values = get_series_values(sa_data, prefer_second_col=True, fill_value=fill_value)

    if len(ga_values) == 0 or len(sa_values) == 0:
        print(f"Warning: Empty data for {metric_name}")
        return

    strategy = get_resample_strategy(metric_name, minimize=minimize)
    sa_gen = resample_sa_to_generations(
        sa_values,
        len(ga_values),
        strategy=strategy,
        fill_value=fill_value,
    )

    ga_plot = rolling_mean(ga_values, smooth_window, fill_value=fill_value)
    sa_plot = rolling_mean(sa_gen, smooth_window, fill_value=fill_value)

    # Final protection against any non-finite values that may still appear.
    ga_plot = sanitize_values(ga_plot, fill_value=fill_value)
    sa_plot = sanitize_values(sa_plot, fill_value=fill_value)

    x = np.arange(len(ga_plot))

    fig, ax = new_fig_ax()
    ax.plot(x, ga_plot, color="blue", label="GA")
    ax.plot(x, sa_plot, color="red", linestyle="--", label="SA")
    ax.set_xlabel("Generations")
    ax.set_ylabel(ylabel)
    ax.legend()

    save_close(fig, output_dir / out_name)


def plot_combined_evalue(pop_dir: Path, traj_dir: Path, output_dir: Path) -> None:
    pop_path = pop_dir / "EValue_P.txt"
    traj_path = traj_dir / "EValue_T.txt"

    if not pop_path.exists() or not traj_path.exists():
        print("Warning: Missing EValue file")
        return

    pop_vals = sanitize_values(np.asarray(load_txt(pop_path), dtype=float).ravel(), fill_value=1.0)
    traj_vals = sanitize_values(np.asarray(load_txt(traj_path), dtype=float).ravel(), fill_value=1.0)

    labels = [r"$\eta_{quality}$", r"$\eta_{conv}$", "EValue"]
    n = min(3, len(pop_vals), len(traj_vals))
    labels = labels[:n]

    x = np.arange(n)
    width = 0.35

    fig, ax = new_fig_ax()
    ax.bar(x - width / 2, pop_vals[:n], width=width, label="GA", color="blue", alpha=0.75)
    ax.bar(x + width / 2, traj_vals[:n], width=width, label="SA", color="red", alpha=0.6)
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_ylabel("Value")
    ax.legend()

    save_close(fig, output_dir / "combinedEValue.png")


def flatten_numeric_file(path: Path) -> np.ndarray:
    values = []
    with open(path, "r", encoding="utf-8", errors="ignore") as fh:
        for line in fh:
            parts = line.strip().split()
            for token in parts:
                try:
                    values.append(float(token))
                except ValueError:
                    pass
    return sanitize_values(np.asarray(values, dtype=float), fill_value=1.0)


def plot_combined_distimp(pop_dir: Path, traj_dir: Path, output_dir: Path) -> None:
    mut_path = pop_dir / "rateOfImprovementMutations.txt"
    cross_path = pop_dir / "rateOfImprovementCrossovers.txt"
    sa_path = traj_dir / "DistImp_T.txt"

    if not mut_path.exists() or not cross_path.exists() or not sa_path.exists():
        print("Warning: Missing files for combined DistImp")
        return

    ga_mut = flatten_numeric_file(mut_path)
    ga_cross = flatten_numeric_file(cross_path)
    sa_vals = flatten_numeric_file(sa_path)

    fig, ax = new_fig_ax()
    bp = ax.boxplot(
        [ga_mut, ga_cross, sa_vals],
        labels=["GA-Mut", "GA-Cross", "SA"],
        patch_artist=True,
    )

    colors = ["blue", "lightskyblue", "red"]
    for box, color in zip(bp["boxes"], colors):
        box.set_facecolor(color)
        box.set_alpha(0.55)

    ax.set_ylabel("Distribution of Improvements")
    save_close(fig, output_dir / "combinedDistImp.png")


def plot_combined_distdet(pop_dir: Path, traj_dir: Path, output_dir: Path) -> None:
    mut_path = pop_dir / "rateOfDeteriorationMutations.txt"
    cross_path = pop_dir / "rateOfDeteriorationCrossovers.txt"
    sa_path = traj_dir / "DistDet_T.txt"

    if not mut_path.exists() or not cross_path.exists() or not sa_path.exists():
        print("Warning: Missing files for combined DistDet")
        return

    ga_mut = flatten_numeric_file(mut_path)
    ga_cross = flatten_numeric_file(cross_path)
    sa_vals = flatten_numeric_file(sa_path)

    fig, ax = new_fig_ax()
    bp = ax.boxplot(
        [ga_mut, ga_cross, sa_vals],
        labels=["GA-Mut", "GA-Cross", "SA"],
        patch_artist=True,
    )

    colors = ["blue", "lightskyblue", "red"]
    for box, color in zip(bp["boxes"], colors):
        box.set_facecolor(color)
        box.set_alpha(0.55)

    ax.set_ylabel("Distribution of Deteriorations")
    save_close(fig, output_dir / "combinedDistDet.png")


def plot_combined_sdistance(pop_dir: Path, traj_dir: Path, output_dir: Path) -> None:
    pop_path = pop_dir / "SDistance_P.txt"
    traj_path = traj_dir / "SDistance_T.txt"

    if not pop_path.exists() or not traj_path.exists():
        print("Warning: Missing SDistance file")
        return

    ga_vals = flatten_numeric_file(pop_path)
    sa_vals = flatten_numeric_file(traj_path)

    fig, ax = new_fig_ax()
    bp = ax.boxplot(
        [ga_vals, sa_vals],
        labels=["GA", "SA"],
        patch_artist=True,
    )

    colors = ["blue", "red"]
    for box, color in zip(bp["boxes"], colors):
        box.set_facecolor(color)
        box.set_alpha(0.55)

    ax.set_ylabel("SDistance")
    save_close(fig, output_dir / "combinedSDistance.png")


def plot_combined_summary(pop_dir: Path, traj_dir: Path, output_dir: Path, smooth_window: int, minimize: bool) -> None:
    fig, axes = plt.subplots(2, 2, figsize=(12, 8), constrained_layout=True)
    for ax in axes.ravel():
        ax.grid(True, linestyle="--", alpha=0.7)

    plot_specs = [
        ("CurrentB_P.txt", "CurrentB_T.txt", "CurrentB", "Objective Function", axes[0, 0]),
        ("ConvRate_P.txt", "ConvRate_T.txt", "ConvRate", "Convergence Rate", axes[0, 1]),
        ("EntropyDiv_P.txt", "EntropyDiv_T.txt", "EntropyDiv", "Entropy Diversity", axes[1, 0]),
        ("HamDist_P.txt", "HamDist_T.txt", "HamDist", "HamDist", axes[1, 1]),
    ]

    for pop_file, traj_file, metric_name, ylabel, ax in plot_specs:
        pop_path = pop_dir / pop_file
        traj_path = traj_dir / traj_file
        if not pop_path.exists() or not traj_path.exists():
            continue

        fill_value = get_fill_value(metric_name)
        ga_values = get_series_values(load_txt(pop_path), prefer_second_col=True, fill_value=fill_value)
        sa_values = get_series_values(load_txt(traj_path), prefer_second_col=True, fill_value=fill_value)
        strategy = get_resample_strategy(metric_name, minimize=minimize)
        sa_gen = resample_sa_to_generations(sa_values, len(ga_values), strategy, fill_value=fill_value)
        ga_plot = rolling_mean(ga_values, smooth_window, fill_value=fill_value)
        sa_plot = rolling_mean(sa_gen, smooth_window, fill_value=fill_value)

        ga_plot = sanitize_values(ga_plot, fill_value=fill_value)
        sa_plot = sanitize_values(sa_plot, fill_value=fill_value)

        x = np.arange(len(ga_plot))
        ax.plot(x, ga_plot, color="blue", label="GA")
        ax.plot(x, sa_plot, color="red", linestyle="--", label="SA")
        ax.set_xlabel("Generations")
        ax.set_ylabel(ylabel)
        ax.set_title(metric_name)

    handles, labels = axes[0, 0].get_legend_handles_labels()
    if handles:
        fig.legend(handles, labels, loc="upper center", ncol=2)

    save_close(fig, output_dir / "combined_summary.png")


def main_combined(pop_dir: Path, traj_dir: Path, smooth_window: int, minimize: bool) -> None:
    output_dir = pop_dir / "combined_figures"
    output_dir.mkdir(parents=True, exist_ok=True)

    plot_combined_line_generation_axis(
        pop_dir, traj_dir, output_dir,
        "ConvGraph_P.txt", "ConvGraph_T.txt",
        "combined_ConvGraph.png", "ConvGraph", "Objective Function",
        smooth_window=smooth_window, minimize=minimize
    )
    plot_combined_line_generation_axis(
        pop_dir, traj_dir, output_dir,
        "CurrentB_P.txt", "CurrentB_T.txt",
        "combinedCurrentB.png", "CurrentB", "Objective Function",
        smooth_window=smooth_window, minimize=minimize
    )

    for pop_file, traj_file, out_name, metric_name, ylabel in [
        ("ASID_P.txt", "ASID_T.txt", "combinedASID.png", "ASID", "ASID"),
        ("ConvRate_P.txt", "ConvRate_T.txt", "combinedConvRate.png", "ConvRate", "Convergence Rate"),
        ("GeoConvRate_P.txt", "GeoConvRate_T.txt", "combinedGeoConvRate.png", "GeoConvRate", "GCR"),
        ("HamDist_P.txt", "HamDist_T.txt", "combinedHamDist.png", "HamDist", "HamDist"),
        ("RelError_P.txt", "RelError_T.txt", "combinedRelError.png", "RelError", "Relative Error"),
        ("EntropyDiv_P.txt", "EntropyDiv_T.txt", "combinedEntropyDiv.png", "EntropyDiv", "Entropy Diversity"),
    ]:
        plot_combined_line_generation_axis(
            pop_dir, traj_dir, output_dir,
            pop_file, traj_file, out_name, metric_name, ylabel,
            smooth_window=smooth_window, minimize=minimize
        )

    plot_combined_evalue(pop_dir, traj_dir, output_dir)
    plot_combined_distimp(pop_dir, traj_dir, output_dir)
    plot_combined_distdet(pop_dir, traj_dir, output_dir)
    plot_combined_sdistance(pop_dir, traj_dir, output_dir)
    plot_combined_summary(pop_dir, traj_dir, output_dir, smooth_window, minimize)

    print(f"Combined figures saved in: {output_dir}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Create combined GA/SA plots on a generation axis by resampling SA from evaluations."
    )
    parser.add_argument(
        "mode",
        choices=["combined"],
        help="Plot mode. This script currently implements the combined mode.",
    )
    parser.add_argument(
        "--pop_dir",
        required=True,
        help="Directory containing GA (_P) files.",
    )
    parser.add_argument(
        "--traj_dir",
        required=True,
        help="Directory containing SA (_T) files.",
    )
    parser.add_argument(
        "--window",
        type=int,
        default=1,
        help="Rolling mean window size applied after alignment. Default: 1.",
    )
    parser.add_argument(
        "--minimize",
        action="store_true",
        help="Use minimization semantics for best-so-far metrics.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    if args.mode == "combined":
        main_combined(
            pop_dir=Path(args.pop_dir),
            traj_dir=Path(args.traj_dir),
            smooth_window=max(1, args.window),
            minimize=args.minimize,
        )


if __name__ == "__main__":
    main()
