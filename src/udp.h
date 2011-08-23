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

#ifndef _UDP__H_
#define _UDP__H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Initialize socket for UDP communication
 *
 * Basic initialization of a socket for further
 * use. Pass the result to udpSend().
 *
 * @return SOCK_DGRAM socket file descriptor.
 */
int udpInitialize(void);

/**
 * Send a datagram using UDP
 *
 * Sends first \c messageSize bytes from \c message buffer
 * to a remote host specified by the \c address and \c port.
 * Unreilable User Datagram Protocol is used.
 *
 * @param[in] udpSocket   Initialized socket file descriptor (@see udpInitialize())
 * @param[in] address     Remote host IP address
 * @param[in] port        Remote host port number
 * @param[in] message     Message content buffer (must be >= messageSize)
 * @param[in] messageSize Size of the message in buffer
 *
 * @return Number of successfully transfered bytes
 */
size_t udpSend(int udpSocket, in_addr_t address, in_port_t port, void *message, size_t messageSize);

/**
 * Close UDP socket
 *
 * Close UDP socket and free any resources
 * allocated by udpInitialize().
 *
 * @param[in] udpSocket Socket file descriptor
 */
void udpClose(int udpSocket);

#endif

