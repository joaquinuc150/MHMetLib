import matplotlib.pyplot as plt
import numpy as np
import argparse
import os
import glob

MINIMIZE = False  #Define according the objective of the problem

plt.rcParams.update({
    "figure.dpi": 120,
    "savefig.dpi": 300,
    "font.size": 12,
    "axes.labelsize": 12,
    "xtick.labelsize": 11,
    "ytick.labelsize": 11,
    "legend.fontsize": 11,
    "lines.linewidth": 0.9,
})

DEFAULT_FIGSIZE = (10, 6)


def color_from_filename(filename: str) -> str:
    """_P -> azul, _T -> rojo, si no, gris."""
    base = os.path.basename(filename)
    if "_P" in base:
        return "blue"
    if "_T" in base:
        return "red"
    return "gray"

def new_fig_ax(figsize=DEFAULT_FIGSIZE):
    """Crea figura+ax consistente y evita estado global sucio."""
    fig, ax = plt.subplots(figsize=figsize, constrained_layout=True)
    ax.grid(True, linestyle="--", alpha=0.7)
    return fig, ax

def save_close(fig, outpath: str):
    """Guarda SIEMPRE igual para evitar recortes y tamaños distintos."""
    fig.savefig(outpath, dpi=300, bbox_inches="tight", pad_inches=0.15)
    plt.close(fig)

def mainPopulationBased(input_dir):
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    try:
        infile = os.path.join(input_dir, 'ConvGraph_P.txt')
        outfile = os.path.join(output_dir, 'ConvGraph_P.png')
        data = np.loadtxt(infile)
        fig, ax = new_fig_ax()
        ax.plot(data[:, 0], data[:, 1], color=color_from_filename(infile))
        ax.set_xlabel('Generation')
        ax.set_ylabel('Objective Function')
        save_close(fig, outfile)
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvGraph_P.txt')}")


    try:
        infile = os.path.join(input_dir, 'HamDist_P.txt')
        outfile = os.path.join(output_dir, 'HamDist_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('HamDist')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'HamDist_P.txt')}")


    try:
        infile = os.path.join(input_dir, 'CurrentB_P.txt')
        outfile = os.path.join(output_dir, 'CurrentB_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('Objective Function')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'CurrentB_P.txt')}")

    try:
        infile = os.path.join(input_dir, 'DistImp_P.txt')
        outfile = os.path.join(output_dir, 'DistImp_P.png')

        data = np.loadtxt(infile)
        non_zero_improvements = data[data[:, 1] != 0, 1]

        fig, ax = new_fig_ax()

        if len(non_zero_improvements) > 0:
            bp = ax.boxplot(
                [non_zero_improvements],
                labels=['GA'],
                patch_artist=True
            )
    
            algo_color = color_from_filename(infile)
            for box in bp['boxes']:
                box.set_facecolor(algo_color)
                box.set_alpha(0.6)

        ax.set_ylabel('DistImp')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistImp_P.txt')}")

    try:
        infile = os.path.join(input_dir, 'DistDet_P.txt')
        outfile = os.path.join(output_dir, 'DistDet_P.png')

        data = np.loadtxt(infile)
        non_zero_deteriorations = data[data[:, 1] != 0, 1]

        fig, ax = new_fig_ax()

        if len(non_zero_deteriorations) > 0:
            bp = ax.boxplot(
                [non_zero_deteriorations],
                labels=['GA'],
                patch_artist=True
            )
    
            algo_color = color_from_filename(infile)
            for box in bp['boxes']:
                box.set_facecolor(algo_color)
                box.set_alpha(0.6)

        ax.set_ylabel('DistDet')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistDet_P.txt')}")


    try:
        infile = os.path.join(input_dir, 'ConvRate_P.txt')
        outfile = os.path.join(output_dir, 'ConvRate_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('Convergence Rate')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRate_P.txt')}")

    try:
        infile = os.path.join(input_dir, 'ConvGraph_P.txt')
        outfile = os.path.join(output_dir, 'ConvGraph_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('Objective Function')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvGraph_P.txt')}")

    try:
        infile = os.path.join(input_dir, 'RelError_P.txt')
        outfile = os.path.join(output_dir, 'RelError_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('Relative Error')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'RelError_P.txt')}")


    try:
        infile = os.path.join(input_dir, 'GeoConvRate_P.txt')
        outfile = os.path.join(output_dir, 'GeoConvRate_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('GCR')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'GeoConvRate_P.txt')}")
    
    try:
        infile = os.path.join(input_dir, 'EntropyDiv_P.txt')
        outfile = os.path.join(output_dir, 'EntropyDiv_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('Entropy Diversity')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyDiv_P.txt')}")
    

    try:
        infile1 = os.path.join(input_dir, 'rateOfImprovementMutations.txt')
        infile2 = os.path.join(input_dir, 'rateOfImprovementCrossovers.txt')
        outfile = os.path.join(output_dir, 'rateOfImprovement.png')

        data1 = np.loadtxt(infile1)
        data2 = np.loadtxt(infile2)

        fig, ax = new_fig_ax()

        bp = ax.boxplot(
            [data1, data2],
            labels=['Mutations', 'Crossovers'],
            patch_artist=True
        )

        colors = ['lightblue', 'lightgreen']
        for box, color in zip(bp['boxes'], colors):
            box.set_facecolor(color)
            box.set_alpha(0.7)

        ax.set_ylabel('Rate of Improvement')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find rate of improvement files")

    try:
        infile1 = os.path.join(input_dir, 'rateOfDeteriorationMutations.txt')
        infile2 = os.path.join(input_dir, 'rateOfDeteriorationCrossovers.txt')
        outfile = os.path.join(output_dir, 'rateOfDeterioration.png')

        data1 = np.loadtxt(infile1)
        data2 = np.loadtxt(infile2)

        fig, ax = new_fig_ax()

        bp = ax.boxplot(
            [data1, data2],
            labels=['Mutations', 'Crossovers'],
            patch_artist=True
        )

        colors = ['lightblue', 'lightgreen']
        for box, color in zip(bp['boxes'], colors):
            box.set_facecolor(color)
            box.set_alpha(0.7)

        ax.set_ylabel('Rate of Deterioration')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find rate of deterioration files")


    try:
        infile = os.path.join(input_dir, 'ASID_P.txt')
        outfile = os.path.join(output_dir, 'ASID_P.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            data[:, 0],
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Generation')
        ax.set_ylabel('ASID')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ASID_P.txt')}")

    try:
        infile = os.path.join(input_dir, 'SDistance_P.txt')
        outfile = os.path.join(output_dir, 'SDistance_P_boxplot.png')

        with open(infile, 'r') as file:
            lines = file.readlines()

        data = [list(map(int, line.strip().split())) for line in lines]

        all_distances = []
        for generation_data in data:
            all_distances.extend(generation_data)

        fig, ax = new_fig_ax()

        bp = ax.boxplot(
            [all_distances],
            labels=['GA'],
            patch_artist=True
        )

        algo_color = color_from_filename(infile)
        for box in bp['boxes']:
            box.set_facecolor(algo_color)
            box.set_alpha(0.6)

        ax.set_ylabel('SDE')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SDistance_P.txt')}")

def mainTrayectorial(input_dir):
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    try:
        infile = os.path.join(input_dir, 'ConvGraph_T.txt')
        outfile = os.path.join(output_dir, 'ConvGraph_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Objective Function')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvGraph_T.txt')}")
    
    try:
        infile = os.path.join(input_dir, 'CurrentB_T.txt')
        outfile = os.path.join(output_dir, 'CurrentB_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Objective Function')

        save_close(fig, outfile)
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'CurrentB_T.txt')}")
    
    try:
        infile = os.path.join(input_dir, 'ASID_T.txt')
        outfile = os.path.join(output_dir, 'ASID_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.shape[0] + 1),
            data[:, 1],
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('ASID')

        save_close(fig, outfile)
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ASID_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'SDistance_T.txt')
        outfile = os.path.join(output_dir, 'SDistance_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax(figsize=(12, 8))

        bp = ax.boxplot(
            data,
            labels=['SA'],
            patch_artist=True
        )

        algo_color = color_from_filename(infile)
        for box in bp['boxes']:
            box.set_facecolor(algo_color)
            box.set_alpha(0.6)

        ax.set_ylabel('SDE')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SDistance_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'ConvRate_T.txt')
        outfile = os.path.join(output_dir, 'ConvRate_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Convergence Rate')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRate_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'DistImp_T.txt')
        outfile = os.path.join(output_dir, 'DistImp_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax(figsize=(12, 8))

        bp = ax.boxplot(
            data,
            labels=['SA'],
            patch_artist=True
        )

        algo_color = color_from_filename(infile)
        for box in bp['boxes']:
            box.set_facecolor(algo_color)
            box.set_alpha(0.6)

        ax.set_ylabel('DistImp')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistImp_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'DistDet_T.txt')
        outfile = os.path.join(output_dir, 'DistDet_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax(figsize=(12, 8))

        bp = ax.boxplot(
            data,
            labels=['SA'],
            patch_artist=True
        )

        algo_color = color_from_filename(infile)
        for box in bp['boxes']:
            box.set_facecolor(algo_color)
            box.set_alpha(0.6)

        ax.set_ylabel('DistDet')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistDet_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'RelError_T.txt')
        outfile = os.path.join(output_dir, 'RelError_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Relative Error')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'RelError_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'GeoConvRate_T.txt')
        outfile = os.path.join(output_dir, 'GeoConvRate_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('GCR')

        save_close(fig, outfile)


    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'GeoConvRate_T.txt')}")

   
    try:
        infile = os.path.join(input_dir, 'HamDist_T.txt')
        outfile = os.path.join(output_dir, 'HamDist_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('HamDist')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'HamDist_T.txt')}")

    try:
        infile = os.path.join(input_dir, 'EntropyDiv_T.txt')
        outfile = os.path.join(output_dir, 'EntropyDiv_T.png')

        data = np.loadtxt(infile)

        fig, ax = new_fig_ax()
        ax.plot(
            np.arange(1, data.size + 1),
            data,
            color=color_from_filename(infile),
            linewidth=1.0
        )

        ax.set_xlabel('Evaluations')
        ax.set_ylabel('Entropy Diversity')

        save_close(fig, outfile)

    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyDiv_T.txt')}")


    

def load_generation_mapping(raw_data_path):
    try:
        raw_data = np.loadtxt(raw_data_path, usecols=(0, 5))
        
        generation_map = {}
        current_gen = None
        start_eval = None
        
        for i, (evaluation, generation) in enumerate(raw_data):
            evaluation = int(evaluation)
            generation = int(generation)
            
            if generation != current_gen:
                if current_gen is not None:
                    generation_map[current_gen] = (start_eval, int(raw_data[i-1, 0]))
                
                current_gen = generation
                start_eval = evaluation
        
        if current_gen is not None:
            generation_map[current_gen] = (start_eval, int(raw_data[-1, 0]))
            
        return generation_map, int(raw_data[-1, 0])
        
    except Exception as e:
        print(f"Error loading generation mapping: {e}")
        return {}, 0

def convert_generation_to_evaluation_based(gen_data, generation_map, total_evals):
    if len(gen_data) == 0 or len(generation_map) == 0:
        return np.array([])
    
    eval_based_data = []
    
    for gen_num, value in gen_data:
        gen_num = int(gen_num)
        if gen_num in generation_map:
            start_eval, end_eval = generation_map[gen_num]
            for eval_num in range(start_eval, end_eval + 1):
                eval_based_data.append([eval_num, value])
    
    return np.array(eval_based_data)


def best_so_far(y, minimize=MINIMIZE):
    y = np.asarray(y).reshape(-1)
    return np.minimum.accumulate(y) if minimize else np.maximum.accumulate(y)




def align_traj_to_pop_x(
    traj_y,
    pop_x,
    generation_map=None,
    minimize=MINIMIZE
):
    """
    Devuelve (x_traj, y_traj_alineado) para graficar el trayectorial en el mismo eje X que el pop.

    - Si pop_x parece ser 'evaluations' (grande), devuelve x=1..len(traj) y y=traj.
    - Si pop_x parece ser 'generations' (pequeño), devuelve x=generations y y=tray muestreado:
        - con generation_map: usa eval de cada gen (mejor)
        - sin map: muestreo uniforme (fallback)
    """
    traj_y = np.asarray(traj_y).reshape(-1)
    pop_x = np.asarray(pop_x).reshape(-1)

    # Heurística: si el último x es mucho menor que las evaluaciones, es "por generación".
    # (ajusta el umbral si lo necesitas)
    pop_last = float(np.nanmax(pop_x)) if pop_x.size else 0
    traj_len = len(traj_y)

    pop_is_generation = pop_last <= max(5, 0.2 * traj_len)

    if not pop_is_generation:
        # pop en evaluaciones → deja trayectorial en evaluaciones
        return np.arange(1, traj_len + 1), traj_y

    # pop en generaciones → alinea trayectorial a generaciones
    gens = pop_x.astype(int)

    y_bsf = best_so_far(traj_y, minimize=MINIMIZE)

    if generation_map:
        # generation_map: dict {gen -> eval} (asumimos evals 1-indexados o 0-indexados: manejamos ambos)
        evals = []
        valid_gens = []
        for g in gens:
            if g in generation_map:
                e = int(generation_map[g])
                # Normaliza a 1..traj_len
                # Si viene 0-indexado, e=0 podría aparecer; lo subimos.
                if e <= 0:
                    e = 1
                if e > traj_len:
                    e = traj_len
                evals.append(e)
                valid_gens.append(g)

        if len(evals) >= 2:
            evals = np.asarray(evals, dtype=int)
            valid_gens = np.asarray(valid_gens, dtype=int)
            return valid_gens, y_bsf[evals - 1]

    # Fallback: muestreo uniforme a la cantidad de generaciones
    if len(gens) == 0:
        return np.arange(1, traj_len + 1), traj_y

    idx = np.linspace(1, traj_len, num=len(gens), dtype=int)
    return gens, y_bsf[idx - 1]

def mainCombined(pop_dir, traj_dir, raw_data_path,minimize=MINIMIZE):
    output_dir = os.path.join(pop_dir, 'combined_figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    raw_data_path = raw_data_path
    generation_map, total_evals = load_generation_mapping(raw_data_path)
    
    if not generation_map:
        print("Warning: Could not load generation mapping. Using original data without alignment.")

    generation_based_files = [
        'CurrentB_P.txt',
        'ConvRate_P.txt', 
        'changeInObjectiveValuePerGeneration.txt',
        'GeoConvRate_P.txt',
        'HamDist_P.txt',
        'RelError_P.txt'
    ]

    plt.figure(figsize=(12, 8))
    try:
        pop_data = np.loadtxt(os.path.join(pop_dir, 'ConvGraph_P.txt'))
        plt.plot(pop_data[:,0], pop_data[:,1], 'b-', label='Pop-based')
    except FileNotFoundError:
        print(f"Warning: Could not find population ConvGraph_P.txt")
    
    try:
        traj_data = np.loadtxt(os.path.join(traj_dir, 'ConvGraph_T.txt'))
        plt.plot(np.arange(1, len(traj_data) + 1), traj_data, 'r--', label='Traj-based')
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory ConvGraph_T.txt")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Objective Function')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_ConvGraph.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))

    pop_x_used = None  # <- guardaremos el eje x con que realmente graficamos el pop

    try:
        pop_best_gen = np.loadtxt(os.path.join(pop_dir, 'CurrentB_P.txt'))

        if generation_map:
            pop_best_eval = convert_generation_to_evaluation_based(pop_best_gen, generation_map, total_evals)
            if len(pop_best_eval) > 0:
                plt.plot(pop_best_eval[:,0], pop_best_eval[:,1], 'b-', label='Pop-based')
                pop_x_used = pop_best_eval[:,0]
            else:
                plt.plot(pop_best_gen[:,0], pop_best_gen[:,1], 'b-', label='Pop-based (by generation)')
                pop_x_used = pop_best_gen[:,0]
        else:
            plt.plot(pop_best_gen[:,0], pop_best_gen[:,1], 'b-', label='Pop-based (by generation)')
            pop_x_used = pop_best_gen[:,0]

    except FileNotFoundError:
        print("Warning: Could not find population best solution data")

    try:
        traj_best = np.loadtxt(os.path.join(traj_dir, 'CurrentB_T.txt'))

        if pop_x_used is None:
            # no pop → deja el tray normal
            plt.plot(np.arange(1, len(traj_best) + 1), traj_best, 'r--', label='Traj-based')
        else:
            x_traj, y_traj = align_traj_to_pop_x(
                traj_best,
                pop_x_used,
                generation_map=generation_map,
                minimize=MINIMIZE  # <- cambia a False si maximizas
            )
            plt.plot(x_traj, y_traj, 'r--', label='Traj-based (aligned)')

    except FileNotFoundError:
        print("Warning: Could not find trajectory best solution data")

    plt.xlabel('Evaluations' if (pop_x_used is not None and np.max(pop_x_used) > 0.2*len(traj_best)) else 'Generations')
    plt.ylabel('Objective Function')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedCurrentB.png'), dpi=300, bbox_inches='tight')
    plt.clf()


    plt.figure(figsize=(12, 8))

    pop_x_used = None
    pop_is_eval = False

    # ---------- POPULATION ----------
    try:
        pop_conv_gen = np.loadtxt(os.path.join(pop_dir, 'ConvRate_P.txt'))

        if generation_map:
            pop_conv_eval = convert_generation_to_evaluation_based(
                pop_conv_gen, generation_map, total_evals
            )
            if len(pop_conv_eval) > 0:
                plt.plot(pop_conv_eval[:,0], pop_conv_eval[:,1],
                         'b-', label='Pop-based')
                pop_x_used = pop_conv_eval[:,0]
                pop_is_eval = True
            else:
                plt.plot(pop_conv_gen[:,0], pop_conv_gen[:,1],
                         'b-', label='Pop-based (by generation)')
                pop_x_used = pop_conv_gen[:,0]
        else:
            plt.plot(pop_conv_gen[:,0], pop_conv_gen[:,1],
                     'b-', label='Pop-based (by generation)')
            pop_x_used = pop_conv_gen[:,0]

    except FileNotFoundError:
        print("Warning: Could not find population convergence data")

    # ---------- TRAJECTORIAL ----------
    try:
        traj_conv = np.loadtxt(os.path.join(traj_dir, 'ConvRate_T.txt'))

        if pop_x_used is None:
            # fallback absoluto
            plt.plot(np.arange(1, len(traj_conv) + 1),
                     traj_conv, 'r--', label='Traj-based')

        elif pop_is_eval:
            # pop ya está en evaluaciones → tray normal
            plt.plot(np.arange(1, len(traj_conv) + 1),
                     traj_conv, 'r--', label='Traj-based')

        else:
            # pop está por generaciones → alineamos tray a generaciones
            x_traj, y_traj = align_traj_to_pop_x(
                traj_conv,
                pop_x_used,
                generation_map=generation_map,
                minimize=None            # usa el global / mainCombined
            )
            plt.plot(x_traj, y_traj,
                     'r--', label='Traj-based (aligned)')

    except FileNotFoundError:
        print("Warning: Could not find trajectory convergence data")

    # ---------- AXES ----------
    if pop_is_eval:
        plt.xlabel('Evaluations')
    else:
        plt.xlabel('Generations')

    plt.ylabel('Convergence Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedConvRate.png'),
                dpi=300, bbox_inches='tight')
    plt.clf()

    
    plt.figure(figsize=(12, 8))
    try:
        pop_div = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        plt.plot(pop_div[:,0], pop_div[:,1], 'b-', label='Pop-based', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population diversity data")
    
    try:
        traj_div = np.loadtxt(os.path.join(traj_dir, 'EntropyDiv_T.txt'))
        plt.plot(np.arange(1, len(traj_div) + 1), traj_div, 'r--', label='Traj-based', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory diversity data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Entropy Diversity')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedEntropyDiv.png'), dpi=300, bbox_inches='tight')
    plt.clf()
    
    
    plt.figure(figsize=(12, 8))
    try:
        pop_geom_gen = np.loadtxt(os.path.join(pop_dir, 'GeoConvRate_P.txt'))
        if generation_map:
            pop_geom_eval = convert_generation_to_evaluation_based(pop_geom_gen, generation_map, total_evals)
            if len(pop_geom_eval) > 0:
                plt.plot(pop_geom_eval[:,0], pop_geom_eval[:,1], 'b-', label='Pop-based')#, linewidth=2)
            else:
                plt.plot(pop_geom_gen[:,0], pop_geom_gen[:,1], 'b-', label='Pop-based (by generation)')#, linewidth=2)
        else:
            plt.plot(pop_geom_gen[:,0], pop_geom_gen[:,1], 'b-', label='Pop-based (by generation)')#, linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population geometric rate data")
    
    try:
        traj_geom = np.loadtxt(os.path.join(traj_dir, 'GeoConvRate_T.txt'))
        plt.plot(np.arange(1, len(traj_geom) + 1), traj_geom, 'r--', label='Traj-based')#, linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory geometric rate data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('GCR')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedGeoConvRate.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_entropy = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        plt.plot(pop_entropy[:,0], pop_entropy[:,1], 'b-', label='Pop-based')#, linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population entropy data")
    
    try:
        traj_entropy = np.loadtxt(os.path.join(traj_dir, 'EntropyDiv_T.txt'))
        plt.plot(np.arange(1, len(traj_entropy) + 1), traj_entropy, 'r--', label='Traj-based')#, linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory entropy data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Entropy Diversity')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedEntropyDiv.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))

    pop_x_used = None
    pop_is_eval = False

    # ---------- POPULATION ----------
    try:
        pop_hamming_gen = np.loadtxt(os.path.join(pop_dir, 'HamDist_P.txt'))

        if generation_map:
            pop_hamming_eval = convert_generation_to_evaluation_based(
                pop_hamming_gen, generation_map, total_evals
            )
            if len(pop_hamming_eval) > 0:
                plt.plot(pop_hamming_eval[:,0], pop_hamming_eval[:,1],
                         'b-', label='Pop-based')
                pop_x_used = pop_hamming_eval[:,0]
                pop_is_eval = True
            else:
                plt.plot(pop_hamming_gen[:,0], pop_hamming_gen[:,1],
                         'b-', label='Pop-based (by generation)')
                pop_x_used = pop_hamming_gen[:,0]
        else:
            plt.plot(pop_hamming_gen[:,0], pop_hamming_gen[:,1],
                     'b-', label='Pop-based (by generation)')
            pop_x_used = pop_hamming_gen[:,0]

    except FileNotFoundError:
        print("Warning: Could not find population hamming distance data")

    # ---------- TRAJECTORIAL ----------
    try:
        traj_hamming = np.loadtxt(os.path.join(traj_dir, 'HamDist_T.txt'))

        if pop_x_used is None:
            plt.plot(np.arange(1, len(traj_hamming) + 1),
                     traj_hamming, 'r--', label='Traj-based')

        elif pop_is_eval:
            # pop está en evaluaciones → tray normal
            plt.plot(np.arange(1, len(traj_hamming) + 1),
                     traj_hamming, 'r--', label='Traj-based')

        else:
            # pop está por generaciones → alinear tray a generaciones
            x_traj, y_traj = align_traj_to_pop_x(
                traj_hamming,
                pop_x_used,
                generation_map=generation_map,
                minimize=None            # usa tu global / mainCombined
            )
            plt.plot(x_traj, y_traj,
                     'r--', label='Traj-based (aligned)')

    except FileNotFoundError:
        print("Warning: Could not find trajectory hamming distance data")

    # ---------- AXES ----------
    plt.xlabel('Evaluations' if pop_is_eval else 'Generations')
    plt.ylabel('HamDist')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedHamDist.png'),
                dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))

    pop_x_used = None
    pop_is_eval = False

    # ---------- POPULATION ----------
    try:
        pop_error_gen = np.loadtxt(os.path.join(pop_dir, 'RelError_P.txt'))

        if generation_map:
            pop_error_eval = convert_generation_to_evaluation_based(
                pop_error_gen, generation_map, total_evals
            )
            if len(pop_error_eval) > 0:
                plt.plot(pop_error_eval[:,0], pop_error_eval[:,1],
                         'b-', label='Pop-based')
                pop_x_used = pop_error_eval[:,0]
                pop_is_eval = True
            else:
                plt.plot(pop_error_gen[:,0], pop_error_gen[:,1],
                         'b-', label='Pop-based (by generation)')
                pop_x_used = pop_error_gen[:,0]
        else:
            plt.plot(pop_error_gen[:,0], pop_error_gen[:,1],
                     'b-', label='Pop-based (by generation)')
            pop_x_used = pop_error_gen[:,0]

    except FileNotFoundError:
        print("Warning: Could not find population error data")

    # ---------- TRAJECTORIAL ----------
    try:
        traj_error = np.loadtxt(os.path.join(traj_dir, 'RelError_T.txt'))

        if pop_x_used is None:
            # fallback absoluto
            plt.plot(np.arange(1, len(traj_error) + 1),
                     traj_error, 'r--', label='Traj-based')

        elif pop_is_eval:
            # pop está en evaluaciones → tray normal
            plt.plot(np.arange(1, len(traj_error) + 1),
                     traj_error, 'r--', label='Traj-based')

        else:
            # pop está por generaciones → alinear tray a generaciones
            x_traj, y_traj = align_traj_to_pop_x(
                traj_error,
                pop_x_used,
                generation_map=generation_map,
                minimize=None            # usa el global / mainCombined
            )
            plt.plot(x_traj, y_traj,
                     'r--', label='Traj-based (aligned)')

    except FileNotFoundError:
        print("Warning: Could not find trajectory error data")

    # ---------- AXES ----------
    plt.xlabel('Evaluations' if pop_is_eval else 'Generations')
    plt.ylabel('Relative Error')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combinedRelError.png'),
                dpi=300, bbox_inches='tight')
    plt.clf()

    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))

    # ============================================================
    # AX1 – Final Objective Value (BAR)
    # ============================================================
    try:
        pop_obj_gen = np.loadtxt(os.path.join(pop_dir, 'CurrentB_P.txt'))
        traj_obj = np.loadtxt(os.path.join(traj_dir, 'CurrentB_T.txt'))

        # Pop: último valor reportado
        pop_final = pop_obj_gen[-1, 1]

        # Traj: último valor (trayectorial es por evaluación)
        traj_final = traj_obj[-1]

        final_values = [pop_final, traj_final]
        algorithms = ['Population', 'Trajectory']
        colors = ['blue', 'red']

        ax1.bar(algorithms, final_values, color=colors, alpha=0.7)
        ax1.set_ylabel('Objective Function')
        ax1.grid(True, linestyle='--', alpha=0.3)

    except Exception:
        ax1.text(0.5, 0.5, 'Data not available',
                 ha='center', va='center', transform=ax1.transAxes)

    # ============================================================
    # AX2 – Convergence Rate (alineado)
    # ============================================================
    try:
        pop_conv_gen = np.loadtxt(os.path.join(pop_dir, 'ConvRate_P.txt'))
        traj_conv = np.loadtxt(os.path.join(traj_dir, 'ConvRate_T.txt'))

        pop_x_used = None
        pop_is_eval = False

        if generation_map:
            pop_conv_eval = convert_generation_to_evaluation_based(
                pop_conv_gen, generation_map, total_evals
            )
            if len(pop_conv_eval) > 0:
                ax2.plot(pop_conv_eval[:,0], pop_conv_eval[:,1],
                         'b-', label='Pop-based')
                pop_x_used = pop_conv_eval[:,0]
                pop_is_eval = True
            else:
                ax2.plot(pop_conv_gen[:,0], pop_conv_gen[:,1],
                         'b-', label='Pop-based')
                pop_x_used = pop_conv_gen[:,0]
        else:
            ax2.plot(pop_conv_gen[:,0], pop_conv_gen[:,1],
                     'b-', label='Pop-based')
            pop_x_used = pop_conv_gen[:,0]

        # ---- Trayectorial ----
        if pop_is_eval:
            ax2.plot(np.arange(1, len(traj_conv) + 1),
                     traj_conv, 'r--', label='Traj-based')
            ax2.set_xlabel('Evaluations')
        else:
            x_traj, y_traj = align_traj_to_pop_x(
                traj_conv,
                pop_x_used,
                generation_map=generation_map,
                minimize=None
            )
            ax2.plot(x_traj, y_traj,
                     'r--', label='Traj-based (aligned)')
            ax2.set_xlabel('Generations')

        ax2.set_ylabel('Convergence Rate')
        ax2.legend()
        ax2.grid(True, linestyle='--', alpha=0.3)

    except Exception:
        ax2.text(0.5, 0.5, 'Convergence data\nnot available',
                 ha='center', va='center', transform=ax2.transAxes)

    # ============================================================
    # AX3 – Entropy Diversity (ya está por evaluación)
    # ============================================================
    try:
        pop_div = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        traj_div = np.loadtxt(os.path.join(traj_dir, 'EntropyDiv_T.txt'))

        ax3.plot(pop_div[:,0], pop_div[:,1],
                 'b-', label='Pop-based')
        ax3.plot(np.arange(1, len(traj_div) + 1),
                 traj_div, 'r--', label='Traj-based')

        ax3.set_xlabel('Evaluations')
        ax3.set_ylabel('Entropy Diversity')
        ax3.legend()
        ax3.grid(True, linestyle='--', alpha=0.3)

    except Exception:
        ax3.text(0.5, 0.5, 'Diversity data\nnot available',
                 ha='center', va='center', transform=ax3.transAxes)

    # ============================================================
    # AX4 – Objective vs Evaluations (ConvGraph)
    # ============================================================
    try:
        pop_obj = np.loadtxt(os.path.join(pop_dir, 'ConvGraph_P.txt'))
        traj_obj = np.loadtxt(os.path.join(traj_dir, 'ConvGraph_T.txt'))

        ax4.plot(pop_obj[:,0], pop_obj[:,1],
                 'b-', label='Pop-based')
        ax4.plot(np.arange(1, len(traj_obj) + 1),
                 traj_obj, 'r--', label='Traj-based')

        ax4.set_xlabel('Evaluations')
        ax4.set_ylabel('Objective Function Value')
        ax4.legend()
        ax4.grid(True, linestyle='--', alpha=0.3)

    except Exception:
        ax4.text(0.5, 0.5, 'Efficiency data\nnot available',
                 ha='center', va='center', transform=ax4.transAxes)

    # ============================================================
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_summary.png'),
                dpi=300, bbox_inches='tight')
    plt.clf()


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate various performance plots from algorithm data files.')
    parser.add_argument('input_dir', type=str, nargs='?', help='Directory containing the input text files (for single algorithm analysis)')
    parser.add_argument('typeOfAnalyzer', type=str, help='Type of analyzer to use: populationBased, trajectoryBased, or combined')
    parser.add_argument('--pop_dir', type=str, help='Directory containing population-based algorithm data (for combined analysis)')
    parser.add_argument('--traj_dir', type=str, help='Directory containing trajectory-based algorithm data (for combined analysis)')
    parser.add_argument('--raw_data', type=str, help='Path to the raw data .dat file (required for combined analysis)')
    args = parser.parse_args()

    if args.typeOfAnalyzer == 'populationBased':
        mainPopulationBased(args.input_dir)
    elif args.typeOfAnalyzer == 'trajectoryBased':
        mainTrayectorial(args.input_dir)
    elif args.typeOfAnalyzer == 'combined':
        if not args.pop_dir or not args.traj_dir:
            print("Error: For combined analysis, both --pop_dir and --traj_dir must be specified")
            print("Usage: python script.py --pop_dir /path/to/population/data --traj_dir /path/to/trajectory/data --raw_data /path/to/raw/data.dat combined")
            exit(1)
        if not args.raw_data:
            print("Error: For combined analysis, --raw_data must be specified")
            print("Usage: python script.py --pop_dir /path/to/population/data --traj_dir /path/to/trajectory/data --raw_data /path/to/raw/data.dat combined")
            exit(1)
        mainCombined(args.pop_dir, args.traj_dir, args.raw_data,MINIMIZE)
    else:
        print("Invalid type of analyzer")
        print("Valid options: populationBased, trajectoryBased, combined")
        exit(1)
