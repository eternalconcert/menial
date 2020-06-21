#!/usr/bin/make

export PYTHONPATH=$PYTHONPATH:src/python:testdata/testhost/:/home/xgwschk/projects/eternalconcert/:/home/xgwschk/projects/eternalconcert/pythonenv/lib/$(PYTHON)/site-packages/

PYTHON=python3.8

SOURCES = $(shell find src/ -name "*.cpp")
BUILD_NUMBER_FILE=deployment/buildno.txt

TEST=0
OUTFILE=build/menial.bin

compile:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o $(OUTFILE) -std=c++11 -pthread -Wall -I /usr/include/$(PYTHON)/ -l $(PYTHON) -I src/include/ -lssl -lcrypto -D TEST=$(TEST)
	$(MAKE) index


test: TEST=1
test: OUTFILE=build/menial_test.bin
test: compile
	@build/menial_test.bin -s

compile_static:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o build/menial.bin -Wall -std=c++11 -O3 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -I src/include/ -Bstatic -L src/include/openssl/ -lssl -lcrypto -ldl
	$(MAKE) index

index:
	@deployment/create_index.sh

serve:
	@DEBUG=1 build/menial.bin menial.json

clean:
	rm -rf build/*
	rm -rf website/build/*
	rm -rf menial.tar.gz

website:
	rm -rf website/build/*
	@mkdir -p website/build/styles
	python website/update_values.py
	anvil -i website/src/ -s website/src/less/ -o website/build/ -t "menial" -v website/hashvalues.json

src:
	tar -zcvf menial.tar.gz src/

docker-image: clean
	docker run -v $(PWD):/menial/ menial_build  # Compiles menial in a container
	$(MAKE) src
	$(MAKE) website
	docker build . -t menial # --build-arg http_proxy=http://proxy:3128
	docker tag menial cloud.canister.io:5000/eternalconcert/menial:latest
	docker push cloud.canister.io:5000/eternalconcert/menial:latest

build-image:
	docker build . -t menial_build -f BuildDockerfile # --build-arg http_proxy=http://10.254.1.64:3128

.PHONY: clean compile serve website src deploy
