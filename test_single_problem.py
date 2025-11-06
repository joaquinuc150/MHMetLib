#!/usr/bin/env python3
"""
Comprehensive test for population-based metrics refactoring
Tests ONE problem with ALL metric variants to verify everything works
"""

import os
import subprocess
import sys

# Test configuration
TEST_CONFIG = {
    'problem_name': 'OneMax',
    'problem_number': 1,
    'dimension': 100,
    'population_size': 10,
    'seed': 42,
    'optimum': 100,
    'n_var': 100
}

def create_config_file(test_name, entropy_zones=None, R=None, distance_to_center=0, hd_variant=0, qthr=0.95, distance_center=None):
    """Create a config file for testing"""

    dat_file = f"JPRuns/{TEST_CONFIG['population_size']}/{TEST_CONFIG['dimension']}/{TEST_CONFIG['seed']}/customea/data_f{TEST_CONFIG['problem_number']}_{TEST_CONFIG['problem_name']}/IOHprofiler_f{TEST_CONFIG['problem_number']}_DIM{TEST_CONFIG['dimension']}.dat"

    config_content = f"""problem_file={dat_file}
optimum={TEST_CONFIG['optimum']}
n_var={TEST_CONFIG['n_var']}
mu={TEST_CONFIG['population_size']}
problem_name={TEST_CONFIG['problem_name']}
distance_to_center={distance_to_center}
hd_variant={hd_variant}
qthr={qthr}
path=JPResults/test_{test_name}/
"""

    # Add optional parameters
    if entropy_zones is not None:
        config_content += f"entropy_zones={entropy_zones}\n"

    if R is not None:
        config_content += f"R={R}\n"

    if distance_center is not None:
        config_content += f"distance_center={distance_center}\n"

    # Config file MUST contain "config_p" in name for population-based
    config_filename = f"config_p_test_{test_name}.cnf"
    with open(config_filename, 'w') as f:
        f.write(config_content)

    return config_filename

def run_test(test_name, config_file):
    """Run the metrics analyzer with a config file"""
    print(f"\n{'='*70}")
    print(f"Running test: {test_name}")
    print(f"{'='*70}")

    cmd = ['./main', config_file]

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)

        if result.returncode == 0:
            print(f"Test '{test_name}' completed successfully")
            return True
        else:
            print(f"Test '{test_name}' FAILED")
            print("STDERR:", result.stderr)
            return False
    except subprocess.TimeoutExpired:
        print(f"Test '{test_name}' TIMEOUT (>5 minutes)")
        return False
    except Exception as e:
        print(f"Test '{test_name}' ERROR: {e}")
        return False

def verify_output_files(test_name):
    """Verify that all expected output files were created"""
    output_dir = f"JPResults/test_{test_name}/"

    expected_files = [
        'ConvGraph_P.txt',
        'CurrentB_P.txt',
        'DistImp_P.txt',
        'DistDet_P.txt',
        'ConvRate_P.txt',
        'classicConvergenceOptimumBased.txt',
        'RelError_P.txt',  # If uncommented in main.cpp
        'convergenceStepsPerIndividual.txt',
        'convergenceSteps2.txt',
        'GeoConvRate_P.txt',
        'EValue_P.txt',
        'HamDist_P.txt',
        'EntropyDiv_P.txt',
        'diversityDistanceToCenter.txt',
        'ASID_P.txt',
        'rateOfModificationsPerOperator.txt',
        'rateOfImprovementMutationsAndCrossovers.txt',
        'rateOfImprovementMutations.txt',
        'rateOfImprovementCrossovers.txt',
        'rateOfDeteriorationMutations.txt',
        'rateOfDeteriorationCrossovers.txt',
        'metric_timings.txt',
        'timing_summary.txt',
    ]

    print(f"\nVerifying output files for '{test_name}':")
    missing_files = []
    found_files = []

    for filename in expected_files:
        filepath = os.path.join(output_dir, filename)
        if os.path.exists(filepath):
            file_size = os.path.getsize(filepath)
            found_files.append(filename)
            print(f"{filename} ({file_size} bytes)")
        else:
            missing_files.append(filename)
            print(f"{filename} MISSING")

    # Check for plots
    figures_dir = os.path.join(output_dir, 'figures')
    if os.path.exists(figures_dir):
        plots = os.listdir(figures_dir)
        print(f"\n  Found {len(plots)} plots in figures/:")
        for plot in sorted(plots)[:10]:  # Show first 10
            print(f"    - {plot}")
        if len(plots) > 10:
            print(f"    ... and {len(plots) - 10} more")
    else:
        print(f"figures/ directory MISSING")

    return len(missing_files) == 0, found_files, missing_files

def main():
    print("="*70)
    print("COMPREHENSIVE TEST - Population-Based Metrics Refactoring")
    print("="*70)
    print(f"\nTest Configuration:")
    print(f"  Problem: {TEST_CONFIG['problem_name']} (f{TEST_CONFIG['problem_number']})")
    print(f"  Dimension: {TEST_CONFIG['n_var']}")
    print(f"  Population Size: {TEST_CONFIG['population_size']}")
    print(f"  Seed: {TEST_CONFIG['seed']}")
    print(f"  Optimum: {TEST_CONFIG['optimum']}")

    # Check if dat file exists
    dat_file = f"JPRuns/{TEST_CONFIG['population_size']}/{TEST_CONFIG['dimension']}/{TEST_CONFIG['seed']}/customea/data_f{TEST_CONFIG['problem_number']}_{TEST_CONFIG['problem_name']}/IOHprofiler_f{TEST_CONFIG['problem_number']}_DIM{TEST_CONFIG['dimension']}.dat"

    if not os.path.exists(dat_file):
        print(f"\nERROR: Data file not found: {dat_file}")
        sys.exit(1)

    print(f"\nData file found: {dat_file}")

    # Test Suite
    tests = []
    results = {}

    # Test 1: Default parameters (let entropy_zones and R be calculated)
    print("\n" + "="*70)
    print("TEST 1: Default Parameters")
    print("  - entropy_zones: auto (ceil(log(n_var)) = ceil(log(100)) = 5)")
    print("  - R: auto (ceil(sqrt(n_var)) = ceil(sqrt(100)) = 10)")
    print("  - distance_to_center: 0 (ZERO_CENTER)")
    print("  - hd_variant: 0 (GLOBAL_BEST)")
    print("="*70)

    config1 = create_config_file('default', distance_to_center=0, hd_variant=0)
    tests.append(('default', config1))

    # Test 2: Custom entropy_zones and R
    print("\n" + "="*70)
    print("TEST 2: Custom Entropy Zones and R")
    print("  - entropy_zones: 16")
    print("  - R: 20")
    print("  - distance_to_center: 0 (ZERO_CENTER)")
    print("  - hd_variant: 1 (FIRST_GEN_BEST)")
    print("="*70)

    config2 = create_config_file('custom_params', entropy_zones=16, R=20, distance_to_center=0, hd_variant=1)
    tests.append(('custom_params', config2))

    # Test 3: Distance to center - BEST_INDIVIDUAL
    print("\n" + "="*70)
    print("TEST 3: Distance to Center - Best Individual")
    print("  - distance_to_center: 1 (BEST_INDIVIDUAL_CENTER)")
    print("  - hd_variant: 2 (PREVIOUS_GEN_BEST)")
    print("="*70)

    config3 = create_config_file('dist_best_individual', distance_to_center=1, hd_variant=2)
    tests.append(('dist_best_individual', config3))

    # Test 4: Distance to center - CUSTOM CENTER
    print("\n" + "="*70)
    print("TEST 4: Distance to Center - Custom Center")
    print("  - distance_to_center: 2 (CUSTOM_CENTER)")
    print("  - distance_center: 50,50,50,... (all 50s)")
    print("  - hd_variant: 0 (GLOBAL_BEST)")
    print("="*70)

    custom_center = ','.join(['50'] * TEST_CONFIG['n_var'])
    config4 = create_config_file('dist_custom_center', distance_to_center=2, hd_variant=0, distance_center=custom_center)
    tests.append(('dist_custom_center', config4))

    # Test 5: Different qthr value
    print("\n" + "="*70)
    print("TEST 5: Different Quality Threshold")
    print("  - qthr: 0.90 (instead of default 0.95)")
    print("="*70)

    config5 = create_config_file('qthr_090', qthr=0.90)
    tests.append(('qthr_090', config5))

    for test_name, config_file in tests:
        success = run_test(test_name, config_file)
        results[test_name] = success

        if success:
            all_files_exist, found_files, missing_files = verify_output_files(test_name)
            results[f"{test_name}_files"] = all_files_exist
        else:
            results[f"{test_name}_files"] = False

    # Final Summary
    print("\n\n" + "="*70)
    print("TEST SUMMARY")
    print("="*70)

    total_tests = len(tests)
    passed_tests = sum(1 for k, v in results.items() if not k.endswith('_files') and v)
    passed_files = sum(1 for k, v in results.items() if k.endswith('_files') and v)

    print(f"\nExecution Results:")
    for test_name, _ in tests:
        status = "PASS" if results[test_name] else "FAIL"
        files_status = "PASS" if results.get(f"{test_name}_files", False) else "FAIL"
        print(f"  {test_name:30s} Execution: {status:8s} Files: {files_status}")

    print(f"\nOverall:")
    print(f"  Tests Executed: {passed_tests}/{total_tests}")
    print(f"  Files Verified: {passed_files}/{total_tests}")

    if passed_tests == total_tests and passed_files == total_tests:
        print("\n" + "="*70)
        print("ALL TESTS PASSED")
        print("="*70)
        print("\nThe refactoring is working correctly!")
        print("All metrics are being calculated and output files are generated.")
        return 0
    else:
        print("\n" + "="*70)
        print("SOME TESTS FAILED")
        print("="*70)
        print("\nPlease check the error messages above.")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)
