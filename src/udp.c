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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/stat.h>

#include "udp.h"

int udpInitialize()
{
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpSocket <= 0)
    {
        perror("Unable to create socket.");
        exit(EXIT_FAILURE);
    }

    return udpSocket;
}

size_t udpSend(int udpSocket, in_addr_t address, in_port_t port, void *message, size_t messageSize)
{
    struct stat info;
    if (fstat(udpSocket, &info) != 0)
    {
        perror("Descriptor is invalid.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in remoteAddress;
    memset(&remoteAddress, 0, sizeof(remoteAddress));

    remoteAddress.sin_family = AF_INET;
    remoteAddress.sin_addr.s_addr = address;
    remoteAddress.sin_port = htons(port);

    ssize_t numberOfBytesSend = sendto(udpSocket, message, messageSize, 0,
                                       (const struct sockaddr *) &remoteAddress, sizeof(remoteAddress));

    return numberOfBytesSend;
}

void udpClose(int udpSocket)
{
    close(udpSocket);
}

