#!/bin/sh
sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=100 --max-requests=0 prepare
sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=100 --max-requests=0 run
sysbench fileio --file-total-size=15G --file-test-mode=rndrw --time=100 --max-requests=0 cleanup