COMPILER=clang
MAIN=src/present.c 
SOURCES=src/server.c src/http.c
LIBS=-pthread

all: $(SOURCES)
	$(COMPILER) $(LIBS) -o present $(MAIN) $(SOURCES)

test:
	$(COMPILER) $(LIBS) -I src/ -o http_test src/test/http_test.c $(SOURCES) 
	./http_test
	rm http_test

clean:
	rm present
