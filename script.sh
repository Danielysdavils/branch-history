#!/usr/bin/env bash

set -e

mkdir -p results/raw

EVENTS="branches,branch-misses,instructions,cycles"
REPEAT=5

run_test() {
    name="$1"
    shift

    echo "Running $name..."
    perf stat -r "$REPEAT" -e "$EVENTS" "$@" > "results/raw/${name}.out" 2> "results/raw/${name}.perf.txt"
}

# Exp 1
run_test "exp1_single" ./bin/exp1_global_history single
run_test "exp1_uncorrelated" ./bin/exp1_global_history uncorrelated
run_test "exp1_correlated" ./bin/exp1_global_history correlated
run_test "exp1_anticorrelated" ./bin/exp1_global_history anticorrelated

# Exp 2
for gap in 0 8 32 64 120; do
    run_test "exp2_correlated_gap_${gap}" ./bin/exp2_global_distance correlated "$gap"
    run_test "exp2_uncorrelated_gap_${gap}" ./bin/exp2_global_distance uncorrelated "$gap"
    run_test "exp2_anticorrelated_gap_${gap}" ./bin/exp2_global_distance anticorrelated "$gap"
done

# Exp 3
for gap in 0 8 32 64; do
    run_test "exp3_random_gap_${gap}" ./bin/exp3_local_history random "$gap"

    for period in 4 8 32 64; do
        run_test "exp3_local_period_${period}_gap_${gap}" ./bin/exp3_local_history local "$period" "$gap"
    done
done

# Exp 4 - Global
for gap in 0 8 32 64 120; do
    run_test "exp4_global_correlated_gap_${gap}" ./bin/exp4_effective_size global correlated "$gap"
    run_test "exp4_global_uncorrelated_gap_${gap}" ./bin/exp4_effective_size global uncorrelated "$gap"
    run_test "exp4_global_anticorrelated_gap_${gap}" ./bin/exp4_effective_size global anticorrelated "$gap"
done

# Exp 4 - Local
for gap in 0 8 32 64; do
    run_test "exp4_random_gap_${gap}" ./bin/exp4_effective_size random "$gap"

    for period in 4 8 32 64; do
        run_test "exp4_local_period_${period}_gap_${gap}" ./bin/exp4_effective_size local "$period" "$gap"
    done
done

echo "Done. Raw results saved in results/raw/"
