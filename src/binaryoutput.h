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

FILE* openOutputFile(char* path);
void writeToOutputFile(FILE* file, void* datagram, size_t datagramSize);
void closeOutputFile(FILE* file);

#endif


