#!/bin/bash
# Executes the runtime in GDB
# Substitutes the absolute path from the container with a path relatively derived from the location of this script
# This allows debugging outside of the Docker container
# Also disables pagination and stopping on SIGUSR1

experiment_directory=$(pwd)
project_directory=$(cd ../../.. && pwd)
binary_directory=$(cd "$project_directory"/bin && pwd)

# Copy Flower Image if not here
if [[ ! -f "./flower.jpg" ]]; then
  cp ../../../tests/sod/bin/flower.jpg ./flower.jpg
fi

if [ "$1" != "-d" ]; then
  PATH="$binary_directory:$PATH" LD_LIBRARY_PATH="$binary_directory:$LD_LIBRARY_PATH" sledgert "$experiment_directory/spec.json" &
  sleep 1
else
  echo "Running under gdb"
fi

# expected_result="$(cat ./expected_result.jpg)"
expected_size="$(find expected_result.jpg -printf "%s")"
success_count=0
total_count=50

# curl -H 'Expect:' -H "Content-Type: image/jpg" --data-binary "@flower.jpg" --output result.jpg localhost:10000

# WIP
# exit

for ((i = 0; i < total_count; i++)); do
  echo "$i"
  ext="$RANDOM"
  curl -H 'Expect:' -H "Content-Type: image/jpg" --data-binary "@flower.jpg" --output "result_$ext.jpg" localhost:10000 2>/dev/null
  actual_size="$(find result_"$ext".jpg -printf "%s")"

  # echo "$result"
  if [[ "$expected_size" == "$actual_size" ]]; then
    success_count=$((success_count + 1))
    rm result.jpg
  else
    echo "FAIL"
    echo "Expected Size:"
    echo "$expected_size"
    echo "==============================================="
    echo "Actual Size:"
    echo "$actual_size"
  fi
done

echo "$success_count / $total_count"

if [ "$1" != "-d" ]; then
  sleep 5
  echo -n "Running Cleanup: "
  rm result_*.jpg
  pkill sledgert >/dev/null 2>/dev/null
  echo "[DONE]"
fi
