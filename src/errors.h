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

#ifndef _ERRORS__H_
#define _ERRORS__H_

#include <error.h>
#include <errno.h>

/** Error codes.
 * 
 * Errors are represented by error codes. This program
 * uses standard C posix defined error codes that are
 * available from errno.h. On top of that, EOK constant
 * was defined to signal no error was encountered.
 *
 * Frequently used:
 *   EOK
 *   EIO     i/o error
 *   EINVAL  invalid argument
 *   ENOENT  file doesn't exist
 *   NOMEM   not enough memory
 *   EACCES  permission denied
 */
typedef int error_t;

#define EOK 0

/** Error reporting function.
 *
 * Function prints error specified error message.
 * The format is
 *
 *     <program_name> : <where> : <reason>
 *
 * This implementation writes to stderr. Change this if
 * you want to handle errors differently (for example
 * log with syslog or suppress reporting entirely).
 *
 * If the supplied \c errorCode is invalid, the reason
 * printed will be "Unknown error"
 *
 * @param[in] errorCode Posix error code
 * @param[in] message String to print as message
 *
 * @return 0
 */
inline void printError(error_t errorCode, char* message)
{
    error(0, errorCode, "%s", message);
}

#endif
