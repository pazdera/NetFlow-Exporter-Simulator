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

#include <arpa/inet.h>

/**
 * Convert ip address from string to in_addr_t
 *
 * Prints warning on error and returns 0.
 * FIXME Possible issue: Zero is also valid IP!
 *
 * @param [in] addressInDotNotation IP to convert in string form
 *
 * @return Converted IP or 0 on failure
 */
in_addr_t convertAddress(const char *addressInDotNotation);

#endif
