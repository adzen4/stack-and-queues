#! /usr/bin/env bash

# Check path length and path for all single_path files for both BFS and DFS
inputs="mazes/maze_7x7_single_path.txt mazes/maze_15x15_single_path.txt"

echo "Checking the path length reported by the solvers for single path mazes..."
./check_maze_solver.sh ./maze_solver_bfs length 0 $inputs
echo
./check_maze_solver.sh ./maze_solver_dfs length 0 $inputs

echo
echo "Checking the actual path in the ascii maze..."
./check_maze_solver.sh ./maze_solver_bfs path 0 $inputs
echo
./check_maze_solver.sh ./maze_solver_dfs path 0 $inputs

# multi path checks
inputs="mazes/maze_7x7_multiple_paths.txt mazes/maze_15x15_multiple_paths.txt"
echo
echo "Checking multi path mazes..."
./check_maze_solver.sh ./maze_solver_bfs length 0 $inputs

echo
./check_maze_solver.sh ./maze_solver_bfs path 0 $inputs

# some special mazes
echo
echo "Checking the path length for special mazes..."
# open maze check (only length as there are multiple correct solutions)
./check_maze_solver.sh ./maze_solver_bfs length 0 mazes/maze_7x7_open.txt

# Check if mazes with no path are handled correctly
./check_maze_solver.sh ./maze_solver_bfs length 1 mazes/maze_impossible.txt

