#!/usr/bin/make

export PYTHONPATH=$PYTHONPATH:src/python:testdata/testhost/:website/pyweb/

PYTHON=python3.8
SOURCES = $(shell find src/ -name "*.cpp")
BUILD_NUMBER_FILE=deployment/buildno.txt

TEST=0
OUTFILE=build/menial.bin
VERSION=0.0.$(shell cat $(BUILD_NUMBER_FILE))


compile:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o $(OUTFILE) -std=c++11 -pthread -Wall -I /usr/include/$(PYTHON)/ -l $(PYTHON) -I src/include/ -lssl -lcrypto -lz -D TEST=$(TEST)
	$(MAKE) index


test: TEST=1
test: OUTFILE=build/menial_test.bin
test: clean compile
	@build/menial_test.bin -s

compile_static:
	@echo $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@mkdir -p build
	g++ $(SOURCES) -o build/menial.bin -Wall -std=c++11 -O3 -static -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -I src/include/ -Bstatic -L src/include/openssl/ -lssl -lcrypto -ldl
	$(MAKE) index

index:
	@deployment/create_index.sh $(VERSION)

serve:
	@DEBUG=1 build/menial.bin menial.json

clean:
	rm -rf build/*
	rm -rf website/build/*
	rm -rf menial.tar.gz

website: deb src
	rm -rf website/build/*
	rm website/src/static/*.deb
	@mkdir -p website/build/styles
	cp build/menial_$(VERSION).deb website/src/static/
	python3 website/update_values.py

	anvil -i website/src/ -s website/src/less/ -o website/build/ -t "menial" -v website/hashvalues.json

src:
	tar -zcvf menial.tar.gz src/

docker-image: clean
	# docker run -v $(PWD):/menial/ menial_build  # Compiles menial in a container
	$(MAKE) website  # creates sources and deb
	docker build . -t menial
	docker tag menial cloud.canister.io:5000/eternalconcert/menial:latest
	docker push cloud.canister.io:5000/eternalconcert/menial:latest

build-image:
	docker build . -t menial_build -f BuildDockerfile

deb: clean compile
	mkdir build/menial_$(VERSION)
	mkdir build/menial_$(VERSION)/DEBIAN
	mkdir build/menial_$(VERSION)/usr
	mkdir build/menial_$(VERSION)/usr/share
	mkdir build/menial_$(VERSION)/usr/share/menial
	mkdir build/menial_$(VERSION)/usr/bin
	mkdir -p build/menial_$(VERSION)/etc/menial
	mkdir -p build/menial_$(VERSION)/usr/share/menial/resources/static/
	mkdir -p build/menial_$(VERSION)/usr/share/menial/http/

	cp build/menial.bin build/menial_$(VERSION)/usr/bin/menial.bin
	cp deployment/menial build/menial_$(VERSION)/usr/bin/menial
	cp deployment/menial.json build/menial_$(VERSION)/etc/menial/
	cp -r deployment/resources/ build/menial_$(VERSION)/usr/share/menial/
	cp -r deployment/default build/menial_$(VERSION)/usr/share/menial/http/


	echo "Package: menial\nVersion: $(VERSION)\nSection: base\nPriority: optional\nArchitecture: amd64\nDepends: python3 (>=3.6), python3-dev (>=3.6)\nDescription: Always at your service!\nMaintainer: Christian Kokoska (info@softcreate.de)" > build/menial_$(VERSION)/DEBIAN/control
	dpkg-deb --build build/menial_$(VERSION)


.PHONY: clean compile serve website src deploy
