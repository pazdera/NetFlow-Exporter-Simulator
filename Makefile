CC=gcc
CFLAGS=-c -g -Wall -pedantic -std=c99
LDFLAGS=
EXECUTABLE=nfgen

SOURCES_DIR=src/
SOURCES=$(addprefix $(SOURCES_DIR), nfgen.c netflow.c udp.c)

OBJECTS=$(SOURCES:.c=.o)


.PHONY: build debug clean install

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

#install: $(EXECUTABLE)
#	cp $(EXECUTABLE) $(INSTALL_PATH)

