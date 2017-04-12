#!/usr/bin/make

server:
	g++ src/server/*.cpp -o build/server.bin -std=c++14 -Wall

client:
	g++ src/client/*.cpp -o build/client.bin -std=c++14 -Wall

compile: server client

serve:
	@build/server.bin 8088

connect:
	@build/client.bin localhost 8088

clean:
	rm build/*

.PHONY: compile serve connect
