#!/usr/bin/env python3
"""
Quick test - Single run to verify basic functionality
Tests: OneMax, dimension 100, population 10, seed 42
"""

import os
import subprocess

# Create simple config
config_content = """problem_file=JPRuns/10/100/42/customea/data_f1_OneMax/IOHprofiler_f1_DIM100.dat
optimum=100
n_var=100
mu=10
problem_name=OneMax
distance_to_center=0
hd_variant=0
qthr=0.95
path=JPResults/quick_test/
"""

# Config file MUST contain "config_p" in name for population-based
with open('config_p_quick_test.cnf', 'w') as f:
    f.write(config_content)

print("="*60)
print("QUICK TEST - Basic Functionality Check")
print("="*60)
print("\nConfiguration:")
print("  Problem: OneMax (f1)")
print("  Dimension: 100")
print("  Population: 10")
print("  Seed: 42")
print("  Output: JPResults/quick_test/")
print("\nRunning...")
print("="*60)

result = subprocess.run(['./main', 'config_p_quick_test.cnf'], capture_output=True, text=True)

if result.returncode == 0:
    print("\n✓ Execution completed successfully!")

    # Check key output files
    output_dir = "JPResults/quick_test/"
    key_files = [
        'ConvGraph_P.txt',
        'CurrentB_P.txt',
        'DistImp_P.txt',
        'DistDet_P.txt',
        'HamDist_P.txt',
        'EntropyDiv_P.txt',
        'ASID_P.txt',
        'EValue_P.txt'
    ]

    print("\nKey output files:")
    all_exist = True
    for f in key_files:
        exists = os.path.exists(os.path.join(output_dir, f))
        status = "✓" if exists else "✗"
        print(f"  {status} {f}")
        if not exists:
            all_exist = False

    # Check plots
    figures_dir = os.path.join(output_dir, 'figures')
    if os.path.exists(figures_dir):
        num_plots = len(os.listdir(figures_dir))
        print(f"\n✓ Generated {num_plots} plots in figures/")
    else:
        print("\n✗ No figures directory found")
        all_exist = False

    if all_exist:
        print("\n" + "="*60)
        print("✓✓✓ QUICK TEST PASSED ✓✓✓")
        print("="*60)
    else:
        print("\n" + "="*60)
        print("✗✗✗ SOME FILES MISSING ✗✗✗")
        print("="*60)
else:
    print("\n✗ Execution FAILED")
    print("\nSTDOUT:")
    print(result.stdout)
    print("\nSTDERR:")
    print(result.stderr)
