#!/bin/bash

kill_runtime() {
  echo -n "Running Cleanup: "
  pkill sledgert >/dev/null 2>/dev/null
 # pkill ab >/dev/null 2>/dev/null
  echo "[DONE]"
}

# rm ../cifar10-out/*.csv

rand10=$(ls -1 ../cifar10-bmp/ | sort -R | head -n 10)

arr=($(echo "$rand10" | tr ' ' '\n'))

echo "Running Samples: "
for (( i=0; i<10; i++ ))
do
    echo "Sample #$i: ${arr[i]}"
#    SLEDGE_SCHEDULER=EDF SLEDGE_SANDBOX_PERF_LOG=../cifar10-out/$i-${arr[i]}.csv SLEDGE_NWORKERS=1 LD_LIBRARY_PATH="$(pwd):$LD_LIBRARY_PATH" ./sledgert ./test_armcifar10.json &
    ab -c 1 -n 500 -p ../cifar10-bmp/${arr[i]} -T "image/bmp" -q localhost:10000/ > /dev/null #../cifar10-out/$i-${arr[i]}.txt 
    sleep 1
    # kill_runtime
done
echo "[DONE]"