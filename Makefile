#!/usr/bin/make

server:
	@mkdir -p build
	g++ src/server/*.cpp src/server/messagehandler/*.cpp src/common/*.cpp -o build/server.bin -std=c++14 -Wall

client:
	g++ src/client/*.cpp src/common/*.cpp -o build/client.bin -std=c++14 -Wall

compile: server client

serve:
	@build/server.bin 8080

clean:
	rm build/*

.PHONY: clean client compile serve server
