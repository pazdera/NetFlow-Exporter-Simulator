/*
 * Copyright (c) 2010  by Radek Pazdera <radek.pazdera@gmail.com>
 *
 * This file is part of nfgen.
 *
 * nfgen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nfgen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with nfgen.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "errors.h"

/* Public interface. */
#include "hosts.h"

enum fsmStates
{
    WHITESPACE,
    ADDRESS,
    COMMENT,
    CARRIAGE_RETURN
};

#define BUFFER_ALLOCATION_UNIT 16
typedef struct
{
    char*  string;
    size_t size;
    int free;
} buffer_t;


/** Initialize buffer_t structure.
 *
 * During the initiailzation stage the buffer is
 * allocated to a size specified by BUFFER_ALLOCATION_UNIT
 * constant. Size and empty is set appropriatelly.
 *
 * @param[out] buffer Buffer structure address
 *
 * @return EOK on success, EMEMORY on malloc failure.
 */
static error_t initializeBuffer(buffer_t* buffer);

/** Apend \c character to \c buffer.
 *
 * Buffer is dynamically allocated. If it runs out
 * of space it allocates additional memory to store
 * the characters.
 *
 * The array is expanded in chunks of size specified
 * by BUFFER_ALLOCATION_UNIT.
 *
 * @param[in] character Character to add to the buffer.
 * @param[in, out] buffer Target buffer_t structure. It must
 *                        be initialized by initializeBuffer()
 *                        before adding new characters.
 *
 * @return EOK on succes, EMEMORY on realloc failure.
 */
static error_t addToBuffer(char character, buffer_t* buffer);

/** Empty buffer.
 *
 * The buffer must be initialized (@see initializeBuffer()).
 * This function doesn't resize or free the memory.
 * Instead it wipes the content and makes the buffer
 * empty.
 *
 * @param[in,out] buffer Initialized buffer.
 */
static void emptyBuffer(buffer_t* buffer);

/** Free allocated memory for buffer.
 *
 * Release memory and reset the structure
 * to it's default state (i.e. reset size and
 * free).
 *
 * @param[in, out] buffer Buffer to be free'd.
 */
static void freeBuffer(buffer_t* buffer);

/** Check if character is a whitespace.
 */
static bool isWhiteSpace(char character);

/** Check if character starts a comment.
 */
static bool startsComment(char character);


in_addr_t convertAddress(const char *addressInDotNotation)
{
  in_addr_t conversionResult = 0;

  if (inet_pton(AF_INET, addressInDotNotation, (void *) &conversionResult) != 1)
  {
    perror("Address conversion failed.");
  }

  return conversionResult;
}

error_t readHostsFromFile(char* filePath, in_addr_t** hosts)
{
    error_t status;
    char character;
    enum fsmStates state = WHITESPACE;

    FILE* hostsFile = fopen(filePath, "r");
    if (hostsFile == NULL)
    {
        return errno; /* set by fopen() */
    }

    buffer_t buffer;
    status = initializeBuffer(&buffer);
    if (status != EOK)
    {
        return status;
    }

    while (!feof(hostsFile))
    {
        character = fgetc(hostsFile);

        switch (state)
        {
            case WHITESPACE:
                if (isWhiteSpace(character))
                {
                    continue;
                }
                else if (startsComment(character))
                {
                    state = COMMENT;
                }
                else
                {
                    state = ADDRESS;
                    addToBuffer(character, &buffer);
                }
                break;

            case ADDRESS:
                if (isWhiteSpace(character))
                {
                    /* TODO Convert address and flush the buffer. */
                    state = WHITESPACE;
                }
                else if (startsComment(character))
                {
                    /* the same */
                    state = COMMENT;
                }
                else
                {
                    addToBuffer(character, &buffer);
                }
                break;

            case COMMENT:
                if (character == '\r')
                {
                    state = CARRIAGE_RETURN;
                }
                else if (character == '\n')
                {
                    state = WHITESPACE;
                }
                else
                {
                    /* Do nothing. */
                }
                break;

            case CARRIAGE_RETURN:
                if (character == '\n')
                {
                    state = WHITESPACE;
                }
                else
                {
                    return EILSEQ;
                }
                break;
        }
    }

    freeBuffer(&buffer);
    fclose(hostsFile);

    return 0;
}

error_t initializeBuffer(buffer_t* buffer)
{
    buffer->string = (char*) malloc(BUFFER_ALLOCATION_UNIT);
    if (buffer->string == NULL)
    {
        return ENOMEM;
    }

    buffer->string[0] = '\0';
    buffer->size  = BUFFER_ALLOCATION_UNIT;
    buffer->free  = buffer->size - 1;

    return EOK;
}

error_t addToBuffer(char character, buffer_t* buffer)
{
    if (!buffer->free)
    {
        /* Buffer is full. Allocate more space. */
        char* newString = realloc(buffer->string, buffer->size + BUFFER_ALLOCATION_UNIT);
        if (newString == NULL)
        {
            return ENOMEM;
        }

        buffer->string = newString;
        buffer->size += BUFFER_ALLOCATION_UNIT;
        buffer->free += BUFFER_ALLOCATION_UNIT;
    }

    int firstFree = buffer->size - buffer->free - 1;

    buffer->string[firstFree] = character;
    buffer->string[firstFree+1] = '\0';
    buffer->free--;

    return EOK;
}

void emptyBuffer(buffer_t* buffer)
{
    buffer->string[0] = '\0';
    buffer->free = buffer->size - 1;
}

void freeBuffer(buffer_t* buffer)
{
    free(buffer->string);

    buffer->string = NULL;
    buffer->size = 0;
    buffer->free = 0;
}

bool isWhiteSpace(char character)
{
    return character == ' '  || character == '\t' ||
           character == '\r' || character == '\n';
}

bool startsComment(char character)
{
    return character == '#';
}

