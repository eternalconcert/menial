#!/bin/bash

OS_INFO=$(uname -mrs)

cp deployment/index_neutral.html deployment/default/index.html
sed -i -- 's/{{ OS_INFO }}/'"${OS_INFO}"'/g' deployment/default/index.html
