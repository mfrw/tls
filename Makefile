GCC=gcc -std=c89 -O3
all:clean obj/des.o

.PHONY:client webserver
client: bin/client
webserver: bin/webserver

bin/client: obj/client.o obj/http.o obj/base64.o
	${GCC} -o $@ obj/client.o obj/http.o obj/base64.o

bin/webserver: obj/webserver.o obj/http.o
	${GCC} -o $@ obj/webserver.o obj/http.o obj/base64.o

obj/client.o: src/client.c include/base64.h include/http.h
	${GCC} -c -o $@ src/client.c -I include

obj/http.o: src/http.c include/http.h include/base64.h
	${GCC} -c -o $@ src/http.c -I include

obj/base64.o: src/base64.c
	${GCC} -c -o $@ src/base64.c -I include

obj/webserver.o: src/webserver.c include/http.h
	${GCC} -c -o $@ src/webserver.c -I include

obj/des.o: src/des.c include/des.h
	${GCC} -c -o $@ src/des.c -I include

.PHONY:clean
clean:
	rm -rf bin/*
	rm -rf obj/*
