#!/usr/bin/make

export PYTHONPATH=$PYTHONPATH:../src/python:../testdata/testhost/

PATH := $(PATH):webdriver/

serve:
	@DEBUG=1 ../build/menial.bin menial.json

tests:
	pythonenv/bin/robot --variable BROWSER:Firefox --timestampoutputs --outputdir=reports testcases
	pythonenv/bin/robot --variable BROWSER:Chrome --timestampoutputs --outputdir=reports testcases

pythonenv:
	virtualenv --python=python2 pythonenv
	pythonenv/bin/pip install robotframework robotframework-seleniumlibrary

clean:
	rm -rf pythonenv/

.PHONY: clean
