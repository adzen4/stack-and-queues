#! /usr/bin/env bash

set -Eeuo pipefail
trap cleanup SIGINT SIGTERM ERR EXIT

TOTAL=0
FAILED=0

#calc(){ awk -v CONVFMT=%.1g "BEGIN { print $*}"; }

cleanup() {
  trap - SIGINT SIGTERM ERR EXIT
  # script cleanup here
  rm -f tmp
  echo -n "score: "
  echo "scale=2; 1.0 - $FAILED/$TOTAL" | bc
  #calc 1 - $FAILED/$TOTAL
  exit 0
}

function run_and_check_length() {
    PROG="$1"
    input="$2"
    ref_output="$3"
    ret_val=${4:-0}

    echo -n "Checking $(basename "$input"): "

    # First check if $PROG returns a 0 error code and bail
    if ! "$PROG" < "$input" > /dev/null 2>&1;
    then
        if [ "$ret_val" -eq 0 ]
        then
            FAILED=$((FAILED + 1))
            echo "returned an error code"
            return 0
        else
            echo "correct, received error as expected"
            return 0
        fi
    else
        if [ "$ret_val" -eq 1 ]
        then
            FAILED=$((FAILED + 1))
            echo "not correct, expected failure"
            return 0
        fi
    fi

    # NOTE: error message is not correct when no path is found with ret val 0
    "$PROG" < "$input" | grep "found a path of length:" > tmp || true
    if grep "found a path of length:" "$ref_output" | diff - tmp > /dev/null;
    then
        echo "correct"
    else
        FAILED=$((FAILED + 1))
        echo "not correct"
        echo "  Your output is:"
        echo -n "  "
        cat tmp
        echo "  The correct output is:"
        echo -n "  "
        head -n 1 "$ref_output"
    fi
}

function run_and_check_path() {
    PROG="$1"
    input="$2"
    ref_output="$3"
    ret_val=${4:-0}

    # First check if $PROG returns a 0 error code and bail
    if ! "$PROG" < "$input" > /dev/null 2>&1;
    then
        if [ "$ret_val" -eq 0 ]
        then
            FAILED=$((FAILED + 1))
            echo "returned an error code"
            return 0
        fi
    else
        if [ "$ret_val" -eq 1 ]
        then
            FAILED=$((FAILED + 1))
            echo "not correct, expected failure"
            return 0
        fi
    fi

    echo -n "Checking $(basename "$input"): "
    if diff <("$PROG" < "$input"  | grep -v "found a path" | sed 's/[^x]/_/g') \
        <(grep -v "found a path" "$ref_output" | sed 's/[^x]/_/g');
    then
        echo "correct"
    else
        FAILED=$((FAILED + 1))
        echo "not correct"
    fi
}

# usage: check_maze_solvers [solver] [method: length/path] [ret_val] [mazes..]
PROG=$1 
METHOD=$2
RET_VAL=$3
shift 3
# Total number of mazes to test.
TOTAL=$#

if [[ $METHOD == "length" ]];
then
    echo "Checking the reported path length for $PROG"
else
    echo "Checking the path for $PROG"
fi

for input in "$@"
do
    #echo $input
    if [[ $PROG == *maze_solver_bfs ]];
    then
        ref="$input.bfs_ref"
    else
        ref="$input.dfs_ref"
    fi
    if [[ $METHOD == "length" ]];
    then
        run_and_check_length "$PROG" "$input" "$ref" "$RET_VAL"
    else
        run_and_check_path "$PROG" "$input" "$ref" "$RET_VAL"
    fi
done
