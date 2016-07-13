CC=gcc
SOURCES= src/present.c src/server.c src/http.c

all:; $(CC) $(SOURCES) -o present
clean:; rm present
