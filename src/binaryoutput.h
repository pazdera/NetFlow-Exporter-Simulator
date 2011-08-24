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

#ifndef _BINARYOUTPUT__H_
#define _BINARYOUTPUT__H_

#include <stdio.h>

/**
 * Open output file and return the handle
 *
 * The file is open in binary mode "w+b". It calls
 * exit() on error.
 *
 * TODO
 *   Calling exit is a bad thing. Only main module should
 *   be able to control the application runtime.
 *
 * @param[in] path Absolute/relative file path
 * @return Open file handle
 */
FILE* openOutputFile(char* path);

/**
 * Write datagram into file
 *
 * Writes \c datagramSize bytes from \c datagram buffer
 * into \c file and flushes the stream.
 *
 * It calls exit() upon error.
 * TODO Fix that.
 *
 * @param[in] file         Open file (@see openOutputFile())
 * @param[in] datagram     Data to be stored into the file
 * @param[in] datagramSize Number of bytes to write
 *
 * @return void
 */
void writeToOutputFile(FILE* file, void* datagram, size_t datagramSize);

/**
 * Close open file handle
 * @param[in] file Open file
 *
 * @return void
 */
void closeOutputFile(FILE* file);

#endif


