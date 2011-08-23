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

#include "binaryoutput.h"

#include <stdlib.h>
#include <stdio.h>

FILE* openOutputFile(char* path)
{
    FILE* file = fopen(path, "w+b");

    if (file == NULL)
    {
        perror("Unable to write to output file.");
        exit(EXIT_FAILURE);
    }

    return file;
}

void writeToOutputFile(FILE* file, void* datagram, size_t datagramSize)
{
    fwrite(datagram, sizeof(char), datagramSize, file);
    fflush(file);

    if (ferror(file))
    {
        perror("Cannot write into output file.");
        exit(EXIT_FAILURE);
    }
}

void closeOutputFile(FILE* file)
{
    fclose(file);
}

