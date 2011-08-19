EXECUTABLE_NAME=nfgen
INSTALL_PATH=/usr/local/bin

.PHONY: build clean install

build:
	gcc -Wall -pedantic -std=c99 nfgen.c -o $(EXECUTABLE_NAME)

clean:
	rm -f $(EXECUTABLE_NAME)

install: build
	cp $(EXECUTABLE_NAME) $(INSTALL_PATH)
