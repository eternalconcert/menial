#!/bin/bash

# Run with
# ./filewatch.sh . make website

check() {
    dir="$1"
    chsum1=`md5deep -r $dir | sort | md5sum | awk '{print $1}'`
    chsum2=$chsum1

    while [ $chsum1 = $chsum2 ]
    do
        sleep 1
        chsum2=`md5deep -r $dir | sort | md5sum | awk '{print $1}'`
    done

    eval ${@:2}
    check $@
}

check $*
