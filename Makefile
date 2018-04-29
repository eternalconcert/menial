#!/usr/bin/make

SOURCES = $(shell find src/ -name "*.cpp")

BUILD_NUMBER_FILE=deployment/buildno.txt

compile:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o build/menial -std=c++14 -pthread -Wall -I src/include/
	$(MAKE) index

index:
	@deployment/create_index.sh

serve:
	@build/menial menial.json

clean:
	rm build/*

.PHONY: clean compile serve
