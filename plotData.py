import matplotlib.pyplot as plt
import numpy as np
import argparse
import os
import glob

def mainPopulationBased(input_dir):
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvGraph_P.txt'))
        plt.figure(figsize=(10, 6))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Evaluation')
        plt.ylabel('Objective Function')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'ConvGraph_P.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvGraph_P.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'CurrentB_P.txt'))
        plt.figure(figsize=(10, 6))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Objective Value Best Solution')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'CurrentB_P.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'CurrentB_P.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistImp_P.txt'))
        non_zero_improvements = data[data[:,1] != 0, 1]

        plt.figure(figsize=(10, 6))
        if len(non_zero_improvements) > 0:
            plt.boxplot([non_zero_improvements], labels=['Improvements'], patch_artist=True,
                        boxprops=dict(facecolor='lightgreen'))
        plt.ylabel('Improvement Magnitude')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'DistImp_P.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistImp_P.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistDet_P.txt'))
        non_zero_deteriorations = data[data[:,1] != 0, 1]

        plt.figure(figsize=(10, 6))
        if len(non_zero_deteriorations) > 0:
            plt.boxplot([non_zero_deteriorations], labels=['Deteriorations'], patch_artist=True,
                        boxprops=dict(facecolor='lightcoral'))
        plt.ylabel('Deterioration Magnitude')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'DistDet_P.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistDet_P.txt')}")

    try:
        with open(os.path.join(input_dir, 'rateOfChangePerIndividualPerGeneration.txt'), 'r') as file:
            lines = file.readlines()
        data = [list(map(float, line.strip().split())) for line in lines]
        generations = np.arange(len(data[0]))
        plt.figure(figsize=(16, 12))
        for i, individual in enumerate(data):
            plt.plot(generations, individual, label=f'Individual {i+1}')
        plt.xlabel('Generation')
        plt.ylabel('Rate of Change')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfChangePerIndividualPerGeneration.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'rateOfChangePerIndividualPerGeneration.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvRate_P.txt'))
        plt.figure()
        plt.plot(data[:,0], data[:,1])
        plt.autoscale()
        plt.xlabel('Generation')
        plt.ylabel('Convergence Optimum Based')
        plt.savefig(os.path.join(output_dir, 'ConvRate_P.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRate_P.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'classicConvergenceOptimumBased.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Convergence using relative error to optimum')
        plt.savefig(os.path.join(output_dir, 'classicConvergenceOptimumBased.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'classicConvergenceOptimumBased.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'RelError_P.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Relative Error')
        plt.title('Relative Error per Generation')
        plt.savefig(os.path.join(output_dir, 'RelError_P.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'RelError_P.txt')}")

    try:
        with open(os.path.join(input_dir, 'convergenceStepsPerIndividual.txt'), 'r') as file:
            lines = file.readlines()
        data = [list(map(float, line.strip().split())) for line in lines]
        generations = np.arange(len(data[0]))
        plt.figure(figsize=(16, 12))
        for i, individual in enumerate(data):
            plt.plot(generations, individual, label=f'Individual {i+1}')
        plt.xlabel('Generation')
        plt.ylabel('Convergence')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'convergenceStepsPerIndividual.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'convergenceStepsPerIndividual.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'convergenceSteps2.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Convergence')
        plt.savefig(os.path.join(output_dir, 'convergenceSteps2.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'convergenceSteps2.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'GeoConvRate_P.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Geometric Rate of Fitness Change')
        plt.savefig(os.path.join(output_dir, 'GeoConvRate_P.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'GeoConvRate_P.txt')}")
    
    try:
        data = np.loadtxt(os.path.join(input_dir, 'EntropyDiv_P.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Entropy Diversity')
        plt.savefig(os.path.join(output_dir, 'EntropyDiv_P.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyDiv_P.txt')}")
    
    try:
        data = np.loadtxt(os.path.join(input_dir, 'rateOfModificationsPerOperator.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        if len(data) == 1:
            data = np.append(data, 0.0)
        labels = ['Mutations', 'Crossovers']
        plt.figure(figsize=(8, 6))
        plt.bar(labels, data)
        plt.xlabel('Operators')
        plt.ylabel('Rate')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfModificationsPerOperator.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'rateOfModificationsPerOperator.txt')}")

    try:
        data1 = np.loadtxt(os.path.join(input_dir, 'rateOfImprovementMutations.txt'))
        data2 = np.loadtxt(os.path.join(input_dir, 'rateOfImprovementCrossovers.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        box_plot = ax.boxplot([data1, data2], labels=['Mutations', 'Crossovers'], patch_artist=True)
        colors = ['lightblue', 'lightgreen']
        for patch, color in zip(box_plot['boxes'], colors):
            patch.set_facecolor(color)
        ax.set_ylabel('Rate of Improvement')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfImprovement.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find rate of improvement files")

    try:
        data1 = np.loadtxt(os.path.join(input_dir, 'rateOfDeteriorationMutations.txt'))
        data2 = np.loadtxt(os.path.join(input_dir, 'rateOfDeteriorationCrossovers.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        box_plot = ax.boxplot([data1, data2], labels=['Mutations', 'Crossovers'], patch_artist=True)
        colors = ['lightblue', 'lightgreen']
        for patch, color in zip(box_plot['boxes'], colors):
            patch.set_facecolor(color)
        ax.set_ylabel('Rate of Deterioration')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfDeterioration.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find rate of deterioration files")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'EValue_P.txt'))
        categories = ['n_quality', 'n_convergence', 'e_value']
        colors = []
        
        for i, value in enumerate(data):
            if value > 1:
                colors.append('green')
            elif value == 1:
                colors.append('yellow')
            else:
                colors.append('red')
        
        fig, ax = plt.subplots(figsize=(8, 6))
        bars = ax.bar(categories, data, color=colors)
        
        for i, (bar, value) in enumerate(zip(bars, data)):
            height = bar.get_height()
            if categories[i] == 'e_value':
                label = 'Good' if value > 1 else 'Neutral' if value == 1 else 'Bad'
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                    f'{value:.2f}\n({label})', ha='center', va='bottom')
            else:
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                    f'{value:.2f}', ha='center', va='bottom')
        
        ax.set_ylabel('Value')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'EValue_P.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EValue_P.txt')}")
    except Exception as e:
        print(f"Error processing e_value.txt: {str(e)}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'ASID_P.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Accumulative Sum')
        plt.savefig(os.path.join(output_dir, 'ASID_P.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ASID_P.txt')}")

    try:
        with open(os.path.join(input_dir, 'SDistance_P.txt'), 'r') as file:
            lines = file.readlines()
        data = [list(map(int, line.strip().split())) for line in lines]

        # Flatten all data from all generations into a single list
        all_distances = []
        for generation_data in data:
            all_distances.extend(generation_data)

        plt.figure(figsize=(10, 6))
        plt.boxplot([all_distances], labels=['All Generations'],
                    patch_artist=True, boxprops=dict(facecolor='lightblue'))
        plt.ylabel('Distance to Center')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'SDistance_P_boxplot.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SDistance_P.txt')}")

def mainTrayectorial(input_dir):
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvGraph_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Fevals')
        plt.savefig(os.path.join(output_dir, 'ConvGraph_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvGraph_T.txt')}")
    
    try:
        data = np.loadtxt(os.path.join(input_dir, 'CurrentB_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Fevals Best')
        plt.savefig(os.path.join(output_dir, 'CurrentB_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'CurrentB_T.txt')}")
    
    try:
        data = np.loadtxt(os.path.join(input_dir, 'ASID_T.txt'))
        plt.plot(np.arange(1, data.shape[0] + 1), data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Sumative Intensify')
        plt.savefig(os.path.join(output_dir, 'ASID_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ASID_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'AccumulatedIntensify_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Accumulate Intensify')
        plt.savefig(os.path.join(output_dir, 'AccumulatedIntensify_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'AccumulatedIntensify_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'EntropyWithSphere.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Entropy With Sphere')
        plt.savefig(os.path.join(output_dir, 'EntropyWithSphere.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyWithSphere.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'SpheresAreas.txt'))
        plt.figure(figsize=(12, 8))
        plt.boxplot(data, labels=['SpheresAreas'])
        plt.ylabel('Spheres Areas')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'SpheresAreas.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SpheresAreas.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvRate_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Convergence Rate')
        plt.savefig(os.path.join(output_dir, 'ConvRate_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRate_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistImp_T.txt'))
        plt.figure(figsize=(12, 8))
        plt.boxplot(data, labels=['DistImp_T'])
        plt.ylabel('Improvement')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'DistImp_T.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistImp_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistDet_T.txt'))
        plt.figure(figsize=(12, 8))
        plt.boxplot(data, labels=['DistDet_T'])
        plt.ylabel('Deterioration')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'DistDet_T.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistDet_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'RelError_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Error Rate')
        plt.savefig(os.path.join(output_dir, 'RelError_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'RelError_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'GeoConvRate_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Geometric Rate')
        plt.savefig(os.path.join(output_dir, 'GeoConvRate_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'GeoConvRate_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'VectorDiversity_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Diversity')
        plt.savefig(os.path.join(output_dir, 'VectorDiversity_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'VectorDiversity_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'HamDist_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Distance Hamming')
        plt.savefig(os.path.join(output_dir, 'HamDist_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'HamDist_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'EntropyDiv_T.txt'))
        plt.plot(np.arange(1, data.size + 1), data)
        plt.xlabel('Iterations')
        plt.ylabel('Entropy Diversity')
        plt.savefig(os.path.join(output_dir, 'EntropyDiv_T.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyDiv_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'EValue_T.txt'))
        categories = ['n_quality', 'n_convergence', 'e_value']
        fig, ax = plt.subplots(figsize=(8, 6))
        table_data = [[f"{data[0]:.2f}"], [f"{data[1]:.2f}"], [f"{data[2]:.2f}"]]
        table = ax.table(cellText=table_data,
                            rowLabels=categories,
                            colLabels=['Value'],
                            cellLoc='center',
                            loc='center')
        table.auto_set_font_size(False)
        table.set_fontsize(12)
        ax.axis('off')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'EValue_T.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EValue_T.txt')}")

    try:
        data = np.loadtxt(os.path.join(input_dir, 'OperatorRate_T.txt'))
        categories = ['Operaror Application Rate', 'Operator Deterioration Rate', 'Operator Improvement Rate']
        fig, ax = plt.subplots(figsize=(8, 6))
        table_data = [[f"{data[0]:.2f}"], [f"{data[1]:.2f}"], [f"{data[2]:.2f}"]]
        table = ax.table(cellText=table_data,
                            rowLabels=categories,
                            colLabels=['Value'],
                            cellLoc='center',
                            loc='center')
        table.auto_set_font_size(False)
        table.set_fontsize(12)
        ax.axis('off')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'OperatorRate_T.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except:
        print(f"Warning: Could not find {os.path.join(input_dir, 'OperatorRate_T.txt')}")

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

def mainCombined(pop_dir, traj_dir, raw_data_path):
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
        'geometricRateofFitnessChangePerGeneration.txt',
        'hammingDistanceBestSolutionPerGeneration.txt',
        'RelError_P.txt'
    ]

    plt.figure(figsize=(12, 8))
    try:
        pop_data = np.loadtxt(os.path.join(pop_dir, 'ConvGraph_P.txt'))
        plt.plot(pop_data[:,0], pop_data[:,1], 'b-', label='Population-based', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population objectiveValueOverTime.txt")
    
    try:
        traj_data = np.loadtxt(os.path.join(traj_dir, 'Fevals.txt'))
        plt.plot(np.arange(1, len(traj_data) + 1), traj_data, 'r--', label='Trajectory-based', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory Fevals.txt")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Objective Function Value')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_objective_evolution.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_best_gen = np.loadtxt(os.path.join(pop_dir, 'CurrentB_P.txt'))
        if generation_map:
            pop_best_eval = convert_generation_to_evaluation_based(pop_best_gen, generation_map, total_evals)
            if len(pop_best_eval) > 0:
                plt.plot(pop_best_eval[:,0], pop_best_eval[:,1], 'b-', label='Population-based Best', linewidth=2)
            else:
                plt.plot(pop_best_gen[:,0], pop_best_gen[:,1], 'b-', label='Population-based Best (by generation)', linewidth=2)
        else:
            plt.plot(pop_best_gen[:,0], pop_best_gen[:,1], 'b-', label='Population-based Best (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population best solution data")
    
    try:
        traj_best = np.loadtxt(os.path.join(traj_dir, 'FevalsBest.txt'))
        plt.plot(np.arange(1, len(traj_best) + 1), traj_best, 'r--', label='Trajectory-based Best', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory best solution data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Best Objective Value')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_best_evolution.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_conv_gen = np.loadtxt(os.path.join(pop_dir, 'ConvRate_P.txt'))
        if generation_map:
            pop_conv_eval = convert_generation_to_evaluation_based(pop_conv_gen, generation_map, total_evals)
            if len(pop_conv_eval) > 0:
                plt.plot(pop_conv_eval[:,0], pop_conv_eval[:,1], 'b-', label='Population Convergence', linewidth=2)
            else:
                plt.plot(pop_conv_gen[:,0], pop_conv_gen[:,1], 'b-', label='Population Convergence (by generation)', linewidth=2)
        else:
            plt.plot(pop_conv_gen[:,0], pop_conv_gen[:,1], 'b-', label='Population Convergence (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population convergence data")
    
    try:
        traj_conv = np.loadtxt(os.path.join(traj_dir, 'ConvRate.txt'))
        plt.plot(np.arange(1, len(traj_conv) + 1), traj_conv, 'r--', label='Trajectory Convergence', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory convergence data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Convergence Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_convergence.png'), dpi=300, bbox_inches='tight')
    plt.clf()
    '''
    plt.figure(figsize=(12, 8))
    try:
        pop_div = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        plt.plot(pop_div[:,0], pop_div[:,1], 'b-', label='Population Entropy Diversity', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population diversity data")
    
    try:
        traj_div = np.loadtxt(os.path.join(traj_dir, 'VectorDiversity.txt'))
        plt.plot(np.arange(1, len(traj_div) + 1), traj_div, 'r--', label='Trajectory Vector Diversity', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory diversity data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Diversity')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_diversity.png'), dpi=300, bbox_inches='tight')
    plt.clf()
    '''
    plt.figure(figsize=(12, 8))
    try:
        pop_change_gen = np.loadtxt(os.path.join(pop_dir, 'changeInObjectiveValuePerGeneration.txt'))
        if generation_map:
            pop_change_eval = convert_generation_to_evaluation_based(pop_change_gen, generation_map, total_evals)
            if len(pop_change_eval) > 0:
                plt.plot(pop_change_eval[:,0], pop_change_eval[:,1], 'b-', label='Population Change Rate', linewidth=2)
            else:
                plt.plot(pop_change_gen[:,0], pop_change_gen[:,1], 'b-', label='Population Change Rate (by generation)', linewidth=2)
        else:
            plt.plot(pop_change_gen[:,0], pop_change_gen[:,1], 'b-', label='Population Change Rate (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population change data")
    
    try:
        traj_imp = np.loadtxt(os.path.join(traj_dir, 'Improvement.txt'))
        plt.plot(np.arange(1, len(traj_imp) + 1), traj_imp, 'r--', label='Trajectory Improvement', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory improvement data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Improvement/Change Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_improvement.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_geom_gen = np.loadtxt(os.path.join(pop_dir, 'geometricRateofFitnessChangePerGeneration.txt'))
        if generation_map:
            pop_geom_eval = convert_generation_to_evaluation_based(pop_geom_gen, generation_map, total_evals)
            if len(pop_geom_eval) > 0:
                plt.plot(pop_geom_eval[:,0], pop_geom_eval[:,1], 'b-', label='Population Geometric Rate', linewidth=2)
            else:
                plt.plot(pop_geom_gen[:,0], pop_geom_gen[:,1], 'b-', label='Population Geometric Rate (by generation)', linewidth=2)
        else:
            plt.plot(pop_geom_gen[:,0], pop_geom_gen[:,1], 'b-', label='Population Geometric Rate (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population geometric rate data")
    
    try:
        traj_geom = np.loadtxt(os.path.join(traj_dir, 'GeomRate.txt'))
        plt.plot(np.arange(1, len(traj_geom) + 1), traj_geom, 'r--', label='Trajectory Geometric Rate', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory geometric rate data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Geometric Rate')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_geometric_rate.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_entropy = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        plt.plot(pop_entropy[:,0], pop_entropy[:,1], 'b-', label='Population Entropy', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population entropy data")
    
    try:
        traj_entropy = np.loadtxt(os.path.join(traj_dir, 'EntropyDiversity.txt'))
        plt.plot(np.arange(1, len(traj_entropy) + 1), traj_entropy, 'r--', label='Trajectory Entropy', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory entropy data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Entropy Diversity')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_entropy_diversity.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_hamming_gen = np.loadtxt(os.path.join(pop_dir, 'hammingDistanceBestSolutionPerGeneration.txt'))
        if generation_map:
            pop_hamming_eval = convert_generation_to_evaluation_based(pop_hamming_gen, generation_map, total_evals)
            if len(pop_hamming_eval) > 0:
                plt.plot(pop_hamming_eval[:,0], pop_hamming_eval[:,1], 'b-', label='Population Hamming Distance', linewidth=2)
            else:
                plt.plot(pop_hamming_gen[:,0], pop_hamming_gen[:,1], 'b-', label='Population Hamming Distance (by generation)', linewidth=2)
        else:
            plt.plot(pop_hamming_gen[:,0], pop_hamming_gen[:,1], 'b-', label='Population Hamming Distance (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population hamming distance data")
    
    try:
        traj_hamming = np.loadtxt(os.path.join(traj_dir, 'DistanceHamming.txt'))
        plt.plot(np.arange(1, len(traj_hamming) + 1), traj_hamming, 'r--', label='Trajectory Hamming Distance', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory hamming distance data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Hamming Distance')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_hamming_distance.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    plt.figure(figsize=(12, 8))
    try:
        pop_error_gen = np.loadtxt(os.path.join(pop_dir, 'RelError_P.txt'))
        if generation_map:
            pop_error_eval = convert_generation_to_evaluation_based(pop_error_gen, generation_map, total_evals)
            if len(pop_error_eval) > 0:
                plt.plot(pop_error_eval[:,0], pop_error_eval[:,1], 'b-', label='Population Relative Error', linewidth=2)
            else:
                plt.plot(pop_error_gen[:,0], pop_error_gen[:,1], 'b-', label='Population Relative Error (by generation)', linewidth=2)
        else:
            plt.plot(pop_error_gen[:,0], pop_error_gen[:,1], 'b-', label='Population Relative Error (by generation)', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find population error data")
    
    try:
        traj_error = np.loadtxt(os.path.join(traj_dir, 'ErrorRate.txt'))
        plt.plot(np.arange(1, len(traj_error) + 1), traj_error, 'r--', label='Trajectory Error Rate', linewidth=2)
    except FileNotFoundError:
        print(f"Warning: Could not find trajectory error data")
    
    plt.xlabel('Evaluations')
    plt.ylabel('Error Rate / Relative Error')
    plt.legend()
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_error_rate.png'), dpi=300, bbox_inches='tight')
    plt.clf()

    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
    
    try:
        pop_obj_gen = np.loadtxt(os.path.join(pop_dir, 'CurrentB_P.txt'))
        traj_obj = np.loadtxt(os.path.join(traj_dir, 'FevalsBest.txt'))
        
        final_values = [pop_obj_gen[-1,1], traj_obj[-1]]
        algorithms = ['Population', 'Trajectory']
        colors = ['blue', 'red']
        
        ax1.bar(algorithms, final_values, color=colors, alpha=0.7)
        ax1.set_ylabel('Final Best Objective')
        ax1.grid(True, linestyle='--', alpha=0.3)
    except:
        ax1.text(0.5, 0.5, 'Data not available', ha='center', va='center', transform=ax1.transAxes)
    
    try:
        pop_conv_gen = np.loadtxt(os.path.join(pop_dir, 'ConvRate_P.txt'))
        traj_conv = np.loadtxt(os.path.join(traj_dir, 'ConvRateOpt.txt'))
        
        if generation_map:
            pop_conv_eval = convert_generation_to_evaluation_based(pop_conv_gen, generation_map, total_evals)
            if len(pop_conv_eval) > 0:
                ax2.plot(pop_conv_eval[:,0], pop_conv_eval[:,1], 'b-', label='Population', linewidth=2)
            else:
                ax2.plot(pop_conv_gen[:,0], pop_conv_gen[:,1], 'b-', label='Population', linewidth=2)
        else:
            ax2.plot(pop_conv_gen[:,0], pop_conv_gen[:,1], 'b-', label='Population', linewidth=2)
            
        ax2.plot(np.arange(1, len(traj_conv) + 1), traj_conv, 'r--', label='Trajectory', linewidth=2)
        ax2.set_xlabel('Evaluations')
        ax2.set_ylabel('Convergence Rate')
        ax2.legend()
        ax2.grid(True, linestyle='--', alpha=0.3)
    except:
        ax2.text(0.5, 0.5, 'Convergence data\nnot available', ha='center', va='center', transform=ax2.transAxes)
    
    try:
        pop_div = np.loadtxt(os.path.join(pop_dir, 'EntropyDiv_P.txt'))
        traj_div = np.loadtxt(os.path.join(traj_dir, 'EntropyDiversity.txt'))
        
        ax3.plot(pop_div[:,0], pop_div[:,1], 'b-', label='Population', linewidth=2)
        ax3.plot(np.arange(1, len(traj_div) + 1), traj_div, 'r--', label='Trajectory', linewidth=2)
        ax3.set_xlabel('Evaluations')
        ax3.set_ylabel('Entropy Diversity')
        ax3.legend()
        ax3.grid(True, linestyle='--', alpha=0.3)
    except:
        ax3.text(0.5, 0.5, 'Diversity data\nnot available', ha='center', va='center', transform=ax3.transAxes)
    
    try:
        pop_obj = np.loadtxt(os.path.join(pop_dir, 'ConvGraph_P.txt'))
        traj_obj = np.loadtxt(os.path.join(traj_dir, 'Fevals.txt'))
        
        ax4.plot(pop_obj[:,0], pop_obj[:,1], 'b-', label='Population', linewidth=2)
        ax4.plot(np.arange(1, len(traj_obj) + 1), traj_obj, 'r--', label='Trajectory', linewidth=2)
        ax4.set_xlabel('Function Evaluations')
        ax4.set_ylabel('Objective Value')
        ax4.legend()
        ax4.grid(True, linestyle='--', alpha=0.3)
    except:
        ax4.text(0.5, 0.5, 'Efficiency data\nnot available', ha='center', va='center', transform=ax4.transAxes)
    
    plt.tight_layout()
    plt.savefig(os.path.join(output_dir, 'combined_summary.png'), dpi=300, bbox_inches='tight')
    plt.clf()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate various performance plots from algorithm data files.')
    parser.add_argument('input_dir', type=str, nargs='?', help='Directory containing the input text files (for single algorithm analysis)')
    parser.add_argument('typeOfAnalyzer', type=str, help='Type of analyzer to use: populationBased, trayectorial, or combined')
    parser.add_argument('--pop_dir', type=str, help='Directory containing population-based algorithm data (for combined analysis)')
    parser.add_argument('--traj_dir', type=str, help='Directory containing trajectory-based algorithm data (for combined analysis)')
    parser.add_argument('--raw_data', type=str, help='Path to the raw data .dat file (required for combined analysis)')
    args = parser.parse_args()

    if args.typeOfAnalyzer == 'populationBased':
        mainPopulationBased(args.input_dir)
    elif args.typeOfAnalyzer == 'trayectorialBased':
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
        mainCombined(args.pop_dir, args.traj_dir, args.raw_data)
    else:
        print("Invalid type of analyzer")
        print("Valid options: populationBased, trayectorialBased, combined")
        exit(1)
