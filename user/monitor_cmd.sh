#!/bin/bash

is_loaded=($(lsmod | grep monitor_blocked_times | wc -l))

if [ "$is_loaded" -gt 0 ]; then
    echo "Removing module monitor_blocked_times"
    rmmod monitor_blocked_times
fi

echo "Loading monitor_blocked_times module"
insmod ../monitor_blocked_times.ko > /dev/null

if [ $? -ne 0 ]; then 
    echo "Can't insert module monitor_blocked_times"
fi

./monitor $@

FOUT=$(date +%s)
mkdir "./out/${FOUT}"
mkdir -p ./out/${FOUT}/tasks/{other,kernel,user}
echo $@ >> "./out/${FOUT}/meta"
./read_debugfs $FOUT

# cd render
# python3 duration.py "../out/${FOUT}/"
# python3 duration.py "../out/test"
# python3 plot_monitored_values.py "../out/${FOUT}/" 400000
# python3 plot_monitored_values.py "../out/${FOUT}/" 100000

rmmod monitor_blocked_times
