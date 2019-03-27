#!/usr/bin/make

PATH := $(PATH):geckodriver/

serve:
	../build/menial.bin menial.json

tests:
	pythonenv/bin/robot --outputdir=reports testcases

pythonenv:
	virtualenv --python=python2 pythonenv
	pythonenv/bin/pip install robotframework robotframework-seleniumlibrary

clean:
	rm -rf pythonenv/

.PHONY: clean
