#!/usr/bin/make

SOURCES = $(shell find src/ -name "*.cpp")

compile:
	@mkdir -p build
	g++ $(SOURCES) -o build/menial -std=c++14 -pthread -Wall -I src/include/

serve:
	@build/menial 8080

clean:
	rm build/*

.PHONY: clean compile serve
