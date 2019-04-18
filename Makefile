#!/usr/bin/make

SOURCES = $(shell find src/ -name "*.cpp")
BUILD_NUMBER_FILE=deployment/buildno.txt

TEST=0
OUTFILE=build/menial.bin

compile:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o $(OUTFILE) -std=c++11 -pthread -Wall -I /usr/include/python2.7 -l python2.7 -I src/include/ -lssl -lcrypto -D TEST=$(TEST)
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
	@build/menial.bin menial.json

clean:
	rm -rf build/*
	rm -rf website/build/*
	rm -rf menial.tar.gz
	rm -rf menial_pkg.tar.gz

website:
	rm -rf website/build/*
	@mkdir -p website/build/styles
	python website/update_values.py
	anvil -i website/src/ -s website/src/less/ -o website/build/ -t "menial" -v website/hashvalues.json

src:
	tar -zcvf menial.tar.gz src/

docker-image:
	docker build . -t menial
	docker tag menial cloud.canister.io:5000/eternalconcert/menial:latest
	docker push cloud.canister.io:5000/eternalconcert/menial:latest

deploy: clean test compile_static src website
	tar -zcvf menial_pkg.tar.gz website/build/ build/menial.bin deployment/default/static/ deployment/Dockerfile
	scp menial_pkg.tar.gz christian@softcreate.de://tmp/
	scp menial.tar.gz christian@softcreate.de://tmp/
	ssh -t christian@softcreate.de "tar -xvf /tmp/menial_pkg.tar.gz -C /tmp/; /home/christian/deploymentscripts/menial.sh"

.PHONY: clean compile serve website src deploy
