#!/bin/bash

# make clean -C ~/linux-5.17
#sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=100 --max-requests=0 prepare


cat bench_cmd.txt | while read cmd ; do
    ./monitor_cmd.sh $cmd
done

# sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=100 --max-requests=0 cleanup
