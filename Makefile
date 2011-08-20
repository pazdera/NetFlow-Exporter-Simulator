SOURCE_DIR=src

EXECUTABLE_NAME=nfgen
INSTALL_PATH=/usr/local/bin/

.PHONY: build debug clean install

build:
	gcc -Wall -pedantic -std=c99 $(SOURCE_DIR)/nfgen.c -o $(EXECUTABLE_NAME)

debug:
	gcc -g -Wall -pedantic -std=c99 $(SOURCE_DIR)/nfgen.c -o $(EXECUTABLE_NAME)

clean:
	rm -f $(EXECUTABLE_NAME)

install: build
	cp $(EXECUTABLE_NAME) $(INSTALL_PATH)
