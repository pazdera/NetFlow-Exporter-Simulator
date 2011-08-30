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

#ifndef _HOSTS__H_
#define _HOSTS__H_

#include "errors.h"

#include <arpa/inet.h>

/**
 * Convert ip address from string to in_addr_t
 *
 * @param[in] addressInDotNotation IP to convert in string form
 * @param[out] address Resulting address. 
 *
 * @return Status code EOK on success, EAFNOSUPPORT on failure
 */
error_t convertAddress(const char *addressInDotNotation, in_addr_t* address);

/**
 * Load hosts from file
 *
 * Reads hosts file and returns an array of ip addresses.
 * The file must be in the following format
 *
 *     # Comment
 *     127.0.0.1
 *     192.168.0.1 # Also a comment
 *
 *     10.0.0.1
 *
 * That means one address per line, blank lines, spaces and
 * everything after '#' up to the end of the line is ignored.
 * 
 * TODO Test with Windows CR-LF line endings.
 *
 * Badly formed addresses are ignored with a warning on stderr.
 *
 * Returned array is allocated within the function and is not
 * free'd. Your responsibility is to correctly release the
 * memory.
 *
 * @param[in] filePath Location of the file
 * @param[out] hosts Pointer to an array of in_addr_t
 *
 * @return Number of addresses read or -1 on error
 */
error_t readHostsFromFile(char* filePath, in_addr_t** hosts);
#endif
