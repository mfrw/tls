all=http

http:bin/http

bin/http:
	gcc -O2 -o bin/http src/http.c -I include


.PHONY:clean
clean:
	rm -rf bin/*

