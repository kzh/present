COMPILER=clang
SOURCES= src/present.c src/server.c src/http.c
LIBS=-pthread

all: $(SOURCES)
	$(COMPILER) $(LIBS) -o present $(SOURCES)

clean:
	rm present
