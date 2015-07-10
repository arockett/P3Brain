#!/bin/sh
for N in 0 1 2 3 4 5 6 7 8 9; do
	for M in 0 1 2 3 4; do
		for E in 0.00 0.01 0.02 0.05 0.10 0.25; do
			qsub -v localN=$M$N,localE=$E doThis.sh
		done
	done
done