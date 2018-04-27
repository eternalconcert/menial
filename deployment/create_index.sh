#!/bin/bash

OS_INFO=$(uname -mrs)
TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')

cp deployment/index.neutral deployment/default/index.html
sed -i -- 's/{{ OS_INFO }}/'"${OS_INFO}"'/g' deployment/default/index.html
sed -i -- 's/{{ TIMESTAMP }}/'"${TIMESTAMP}"'/g' deployment/default/index.html
