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

#include "hosts.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>


in_addr_t convertAddress(const char *addressInDotNotation)
{
  in_addr_t conversionResult = 0;

  if (inet_pton(AF_INET, addressInDotNotation, (void *) &conversionResult) != 1)
  {
    perror("Address conversion failed.");
  }

  return conversionResult;
}
