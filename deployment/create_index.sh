#!/bin/bash

OS_INFO=$(uname -mrs)

cp deployment/index.neutral deployment/default/index.html
sed -i -- 's/{{ OS_INFO }}/'"${OS_INFO}"'/g' deployment/default/index.html
