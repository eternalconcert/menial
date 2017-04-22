#!/usr/bin/make

SOURCES = $(shell find src/ -name "*.cpp")

compile:
	@mkdir -p build
	g++ $(SOURCES) -o build/server.bin -std=c++14 -Wall -static

serve:
	@build/server.bin 8080

clean:
	rm build/*

.PHONY: clean compile serve
