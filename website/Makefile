#!/usr/bin/make


build: clean
	@mkdir -p build/styles
	anvil -i src/ -s src/less/ -o build/ -t "menial"

clean:
	@rm -rf build
