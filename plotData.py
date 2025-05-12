import matplotlib.pyplot as plt
import numpy as np
import argparse
import os
import glob

def mainPopulationBased(input_dir):
    # Create output directory if it doesn't exist
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    # ObjectiveFunction over time
    try:
        data = np.loadtxt(os.path.join(input_dir, 'objectiveValueOverTime.txt'))
        plt.figure(figsize=(10, 6))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Evaluation')
        plt.ylabel('Objective Function')
        plt.title('Objective Function over Time')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'objectiveFunctionOverTime.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'objectiveValueOverTime.txt')}")

    # objectiveValueBestSolutionOverTime
    try:
        data = np.loadtxt(os.path.join(input_dir, 'objectiveValueBestSolutionOverTime.txt'))
        plt.figure(figsize=(10, 6))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Objective Value Best Solution')
        plt.title('Objective Value Best Solution over Time')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'objectiveValueBestSolutionOverTime.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'objectiveValueBestSolutionOverTime.txt')}")

    # changeInObjectiveValuePerGeneration
    try:
        data = np.loadtxt(os.path.join(input_dir, 'changeInObjectiveValuePerGeneration.txt'))
        plt.figure(figsize=(10, 6))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Change in Objective Value')
        plt.title('Change in Objective Value per Generation')
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'changeInObjectiveValuePerGeneration.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'changeInObjectiveValuePerGeneration.txt')}")

    # rateOfChangePerIndividualPerGeneration
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
        plt.title('Individual Rate of Objective value change over generations')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfChangePerIndividualPerGeneration.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'rateOfChangePerIndividualPerGeneration.txt')}")

    # ConvergenceWithOptimum
    try:
        data = np.loadtxt(os.path.join(input_dir, 'convergenceOptimumBased.txt'))
        plt.figure()
        plt.plot(data[:,0], data[:,1])
        plt.autoscale()
        plt.xlabel('Generation')
        plt.ylabel('Convergence Optimum Based')
        plt.title('Convergence Optimum Based per Generation')
        plt.savefig(os.path.join(output_dir, 'convergenceOptimumBased.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'convergenceOptimumBased.txt')}")

    # ClassicConvergenceWithOptimum
    try:
        data = np.loadtxt(os.path.join(input_dir, 'classicConvergenceOptimumBased.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Relative error to optimum')
        plt.title('Relative error per Generation')
        plt.savefig(os.path.join(output_dir, 'classicConvergenceOptimumBased.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'classicConvergenceOptimumBased.txt')}")

    # ConvergenceWithoutOptimum
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
        plt.title('Individual Convergence Over Generations')
        plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'convergenceStepsPerIndividual.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'convergenceStepsPerIndividual.txt')}")

    # convergenceSteps2
    try:
        data = np.loadtxt(os.path.join(input_dir, 'convergenceSteps2.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Convergence')
        plt.title('Convergence without optimum per Generation')
        plt.savefig(os.path.join(output_dir, 'convergenceSteps2.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'convergenceSteps2.txt')}")

    # geometricRateofFitnessChangePerGeneration
    try:
        data = np.loadtxt(os.path.join(input_dir, 'geometricRateOfFitnessChangePerGeneration.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Geometric Rate of Fitness Change')
        plt.title('Geometric Rate of Fitness Change per Generation')
        plt.savefig(os.path.join(output_dir, 'geometricRateOfFitnessChangePerGeneration.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'geometricRateOfFitnessChangePerGeneration.txt')}")
    
    # EntropyDiversity
    try:
        data = np.loadtxt(os.path.join(input_dir, 'entropyDiversity.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Entropy Diversity')
        plt.title('Entropy Diversity per Generation')
        plt.savefig(os.path.join(output_dir, 'entropyDiversity.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'entropyDiversity.txt')}")
    
    # rateOfModificationsPerOperator
    try:
        data = np.loadtxt(os.path.join(input_dir, 'rateOfModificationsPerOperator.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        if len(data) == 1:
            data = np.append(data, 0.0)
        labels = ['Mutations', 'Crossovers']
        plt.figure(figsize=(8, 6))
        plt.bar(labels, data)
        plt.title('Comparison of Mutation and Crossover Rates')
        plt.xlabel('Operators')
        plt.ylabel('Rate')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfModificationsPerOperator.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'rateOfModificationsPerOperator.txt')}")

    # Put both rates of improvement in one plot
    try:
        data1 = np.loadtxt(os.path.join(input_dir, 'rateOfImprovementMutations.txt'))
        data2 = np.loadtxt(os.path.join(input_dir, 'rateOfImprovementCrossovers.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        box_plot = ax.boxplot([data1, data2], labels=['Mutations', 'Crossovers'], patch_artist=True)
        colors = ['lightblue', 'lightgreen']
        for patch, color in zip(box_plot['boxes'], colors):
            patch.set_facecolor(color)
        ax.set_ylabel('Rate of Improvement')
        ax.set_title('Rate of Improvement: Mutations vs Crossovers')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfImprovement.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find rate of improvement files")

    # Put both rates of deterioration in one plot
    try:
        data1 = np.loadtxt(os.path.join(input_dir, 'rateOfDeteriorationMutations.txt'))
        data2 = np.loadtxt(os.path.join(input_dir, 'rateOfDeteriorationCrossovers.txt'))
        fig, ax = plt.subplots(figsize=(10, 6))
        box_plot = ax.boxplot([data1, data2], labels=['Mutations', 'Crossovers'], patch_artist=True)
        colors = ['lightblue', 'lightgreen']
        for patch, color in zip(box_plot['boxes'], colors):
            patch.set_facecolor(color)
        ax.set_ylabel('Rate of Deterioration')
        ax.set_title('Rate of Deterioration: Mutations vs Crossovers')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'rateOfDeterioration.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find rate of deterioration files")

    # E-value
    try:
        data = np.loadtxt(os.path.join(input_dir, 'e_value.txt'))
        
        # Define labels for the three values
        categories = ['n_quality', 'n_convergence', 'e_value']
        
        # Define colors based on thresholds
        colors = []
        
        for i, value in enumerate(data):
            if value > 1:
                colors.append('green')
            elif value == 1:
                colors.append('yellow')
            else:
                colors.append('red')
        
        # Create the figure and plot
        fig, ax = plt.subplots(figsize=(8, 6))
        bars = ax.bar(categories, data, color=colors)
        
        # Add value labels on top of the bars
        for i, (bar, value) in enumerate(zip(bars, data)):
            height = bar.get_height()
            if categories[i] == 'e_value':
                # Only add the Good/Bad/Neutral label for e_value
                label = 'Good' if value > 1 else 'Neutral' if value == 1 else 'Bad'
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                    f'{value:.2f}\n({label})', ha='center', va='bottom')
            else:
                # Just show the value for n_quality and n_convergence
                ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                    f'{value:.2f}', ha='center', va='bottom')
        
        ax.set_ylabel('Value')
        ax.set_title('Analysis Results')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'e_value.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'e_value.txt')}")
    except Exception as e:
        print(f"Error processing e_value.txt: {str(e)}")

    '''
    # e-Value
    try:
        data = np.loadtxt(os.path.join(input_dir, 'e_value.txt'))
        if data > 1:
            color = 'green'
            label = 'Good'
        elif data == 1:
            color = 'yellow'
            label = 'Neutral'
        else:
            color = 'red'
            label = 'Bad'
        fig, ax = plt.subplots(figsize=(6, 6))
        ax.bar('e-Value', data, color=color)
        ax.set_ylabel('e-Value')
        ax.set_title(f'e-Value: {label}')
        ax.grid(True, linestyle='--', alpha=0.7)
        ax.set_facecolor('#f0f0f0')
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'e_value.png'), dpi=300, bbox_inches='tight')
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'e_value.txt')}")
    '''
    #accumSum
    try:
        data = np.loadtxt(os.path.join(input_dir, 'accumSum.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Accumulative Sum')
        plt.title('Accumulative Sum per Generation')
        plt.savefig(os.path.join(output_dir, 'accumSum.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'accumSum.txt')}")

   # diversityDistanceToCenter
    try:
        with open(os.path.join(input_dir, 'diversityDistanceToCenter.txt'), 'r') as file:
            lines = file.readlines()
        data = [list(map(int, line.strip().split())) for line in lines]
        
        # Create a boxplot showing distribution every k generations
        k = 5  # Plot every 5th generation (adjust as needed)
        selected_indices = list(range(0, len(data), k))
        if len(data) - 1 not in selected_indices:
            selected_indices.append(len(data) - 1)  # Always include last generation
        
        selected_data = [data[i] for i in selected_indices]
        labels = [f"Gen {i}" for i in selected_indices]
        
        plt.figure(figsize=(12, 8))
        plt.boxplot(selected_data, labels=labels)
        plt.xlabel('Generation')
        plt.ylabel('Distance to Center')
        plt.title('Distribution of Solutions Distance to Center')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'diversityDistanceToCenter_boxplot.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'diversityDistanceToCenter.txt')}")

    #diversityDistanceToCenter2
    try:
        with open(os.path.join(input_dir, 'diversityDistanceToCenter2.txt'), 'r') as file:
            lines = file.readlines()
        data = [list(map(int, line.strip().split())) for line in lines]
        
        # Create a boxplot showing distribution every k generations
        k = 5  # Plot every 5th generation (adjust as needed)
        selected_indices = list(range(0, len(data), k))
        if len(data) - 1 not in selected_indices:
            selected_indices.append(len(data) - 1)  # Always include last generation
        
        selected_data = [data[i] for i in selected_indices]
        labels = [f"Gen {i}" for i in selected_indices]
        
        plt.figure(figsize=(12, 8))
        plt.boxplot(selected_data, labels=labels)
        plt.xlabel('Generation')
        plt.ylabel('Distance to Center (Best Individual)')
        plt.title('Distribution of Solutions Distance to Best Individual')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'diversityDistanceToCenter2_boxplot.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'diversityDistanceToCenter2.txt')}")

    #hammingDistanceBestSolutionPerGeneration
    try:
        data = np.loadtxt(os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Hamming Distance')
        plt.title('Hamming Distance Best Solution per Generation')
        plt.savefig(os.path.join(output_dir, 'hammingDistanceBestSolutionPerGeneration.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration.txt')}")

    #hammingDistanceBestSolutionPerGeneration2
    try:
        data = np.loadtxt(os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration2.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Hamming Distance')
        plt.title('Hamming Distance Best Solution per Generation')
        plt.savefig(os.path.join(output_dir, 'hammingDistanceBestSolutionPerGeneration2.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration2.txt')}")

    #hammingDistanceBestSolutionPerGeneration3
    try:
        data = np.loadtxt(os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration3.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Generation')
        plt.ylabel('Hamming Distance')
        plt.title('Hamming Distance Best Solution per Generation')
        plt.savefig(os.path.join(output_dir, 'hammingDistanceBestSolutionPerGeneration3.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'hammingDistanceBestSolutionPerGeneration3.txt')}")

def mainTrayectorial(input_dir):
    # Create output directory if it doesn't exist
    output_dir = os.path.join(input_dir, 'figures')
    try:
        os.makedirs(output_dir, exist_ok=True)
    except OSError as e:
        print(f"Error creating output directory: {e}")

    # Fevals
    try:
        data = np.loadtxt(os.path.join(input_dir, 'Fevals.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Fevals')
        plt.title('Fevals per Iteration')
        plt.savefig(os.path.join(output_dir, 'Fevals.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'Fevals.txt')}")
    
    # FevalsBest
    try:
        data = np.loadtxt(os.path.join(input_dir, 'FevalsBest.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Fevals Best')
        plt.title('Fevals Best per Iteration')
        plt.savefig(os.path.join(output_dir, 'FevalsBest.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'FevalsBest.txt')}")
    
    # SumativeIntensify
    try:
        data = np.loadtxt(os.path.join(input_dir, 'SumativeIntensify.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Sumative Intensify')
        plt.title('Sumative Intensify per Iteration')
        plt.savefig(os.path.join(output_dir, 'SumativeIntensify.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SumativeIntensify.txt')}")

    # AccumulatedIntensify
    try:
        data = np.loadtxt(os.path.join(input_dir, 'AccumulatedIntensify.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Accumulate Intensify')
        plt.title('Accumulated Intensify per Iteration')
        plt.savefig(os.path.join(output_dir, 'AccumulatedIntensify.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'AccumulatedIntensify.txt')}")

    # EntropyWithSphere
    try:
        data = np.loadtxt(os.path.join(input_dir, 'EntropyWithSphere.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Entropy With Sphere')
        plt.title('Entropy With Sphere per Iteration')
        plt.savefig(os.path.join(output_dir, 'EntropyWithSphere.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyWithSphere.txt')}")

    # SpheresAreas
    try:
        data = np.loadtxt(os.path.join(input_dir, 'SpheresAreas.txt'))
        plt.figure(figsize=(12, 8))
        plt.boxplot(data, labels=['SpheresAreas'])
        plt.ylabel('Spheres Areas')
        plt.title('Distribution of Solutions Spheres Areas')
        plt.xticks(rotation=45)
        plt.grid(True, linestyle='--', alpha=0.7)
        plt.tight_layout()
        plt.savefig(os.path.join(output_dir, 'SpheresAreas_boxplot.png'), dpi=300)
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'SpheresAreas.txt')}")

    # ConvRate
    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvRate.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Convergence Rate')
        plt.title('Convergence Rate per Iteration')
        plt.savefig(os.path.join(output_dir, 'ConvRate.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRate.txt')}")

    # ConvRateOpt
    try:
        data = np.loadtxt(os.path.join(input_dir, 'ConvRateOpt.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Convergence Rate with Optimum')
        plt.title('Convergence Rate with Optimum per Iteration')
        plt.savefig(os.path.join(output_dir, 'ConvRateOpt.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ConvRateOpt.txt')}")

    # Improvement
    try:
        data = np.loadtxt(os.path.join(input_dir, 'Improvement.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Improvement')
        plt.title('Improvement per Iteration')
        plt.savefig(os.path.join(output_dir, 'Improvement.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'Improvement.txt')}")

    # ErrorRate
    try:
        data = np.loadtxt(os.path.join(input_dir, 'ErrorRate.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Error Rate')
        plt.title('Error Rate per Iteration')
        plt.savefig(os.path.join(output_dir, 'ErrorRate.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'ErrorRate.txt')}")

    # GeomRate
    try:
        data = np.loadtxt(os.path.join(input_dir, 'GeomRate.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Geometric Rate')
        plt.title('Geometric Rate per Iteration')
        plt.savefig(os.path.join(output_dir, 'GeomRate.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'GeomRate.txt')}")

    # VectorDiversity
    try:
        data = np.loadtxt(os.path.join(input_dir, 'VectorDiversity.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Diversity')
        plt.title('Diversity per Iteration')
        plt.savefig(os.path.join(output_dir, 'VectorDiversity.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'VectorDiversity.txt')}")

    # DistanceHammingfromZero
    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistanceHammingfromZero.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Distance Hamming from Zero')
        plt.title('Distance Hamming from Zero per Iteration')
        plt.savefig(os.path.join(output_dir, 'DistanceHammingfromZero.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistanceHammingfromZero.txt')}")

    # DistanceHamming
    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistanceHamming.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Distance Hamming')
        plt.title('Distance Hamming per Iteration')
        plt.savefig(os.path.join(output_dir, 'DistanceHamming.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistanceHamming.txt')}")
    
    # DistanceHammingFromLocalBest
    try:
        data = np.loadtxt(os.path.join(input_dir, 'DistanceHammingFromLocalBest.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Distance Hamming from Local Best')
        plt.title('Distance Hamming from Local Best per Iteration')
        plt.savefig(os.path.join(output_dir, 'DistanceHammingFromLocalBest.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'DistanceHammingFromLocalBest.txt')}")

    # EntropyDiversity
    try:
        data = np.loadtxt(os.path.join(input_dir, 'EntropyDiversity.txt'))
        plt.plot(data[:,0], data[:,1])
        plt.xlabel('Iterations')
        plt.ylabel('Entropy Diversity')
        plt.title('Entropy Diversity per Iteration')
        plt.savefig(os.path.join(output_dir, 'EntropyDiversity.png'))
        plt.clf()
    except FileNotFoundError:
        print(f"Warning: Could not find {os.path.join(input_dir, 'EntropyDiversity.txt')}")

    return
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Generate various performance plots from algorithm data files.'
    )
    parser.add_argument('input_dir', type=str, nargs='?',
                        help='Directory containing the input text files')
    parser.add_argument('typeOfAnalyzer', type=str,
                        help='Type of analyzer to use')
    args = parser.parse_args()

    if args.typeOfAnalyzer == 'populationBased':
        mainPopulationBased(args.input_dir)
    elif args.typeOfAnalyzer == 'trayectorial':
        mainTrayectorial(args.input_dir)
    else:
        print("Invalid type of analyzer")
        print(args.typeOfAnalyzer)
        print(args.input_dir)
        exit(1)