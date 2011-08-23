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

#include "netflow.h"

#define MIN_FLOW_DURATION 1
#define MAX_FLOW_DURATION 60

/* Here goes some addresses that will be used as source and destination in netflow records. */
/* TODO Maybe load them from a file? */
#define NUMBER_OF_ADDRESSES 4
const char *addresses[NUMBER_OF_ADDRESSES] =
{
  "127.0.0.1",
  "192.168.1.100",
  "192.168.1.101",
  "192.168.1.102"
};

in_addr_t convertAddress(const char *addressInDotNotation)
{
  in_addr_t conversionResult;

  if (inet_pton(AF_INET, addressInDotNotation, (void *) &conversionResult) != 1)
  {
    perror("Address conversion failed.");
  }

  return conversionResult;
}

/* TODO This could be more sophisticated */
/* Is 'generate' really the right name? */
in_addr_t generateRandomAddress()
{
  return convertAddress(addresses[(1 + rand()) % NUMBER_OF_ADDRESSES]);
}

in_port_t generateRandomPortNumber()
{
  return rand() % 65536;
}

char generateRandomTCPFlags()
{
  return rand() % 255;
}

/* Returns size of the packet in buffer.
   Size of buffer must be greater then 24 + 30*48 = 1464,
   otherwise expect some segfaults. */
size_t makeRandomNetflowPacket(char *buffer, time_t systemStartTime, unsigned int numberOfFlows, unsigned int totalFlowsSent)
{
  time_t currentTime = time(0);
  time_t systemUptime = currentTime - systemStartTime;

  struct netflowRecord record;
  struct netflowHeader header;

  for (int flow = 0;flow < numberOfFlows; flow++)
  {
    // Addresses are already in network byte order
    record.srcAddr = generateRandomAddress();
    record.dstAddr = generateRandomAddress();

    // NIY
    record.nextHop = 0;
    record.input = 0;
    record.output = 0;

    // Some random flow lengths
    record.dPkts = rand() % 100000;
    record.dOctets = record.dPkts * (rand() % 300);
    record.dPkts = htonl(record.dPkts);
    record.dOctets = htonl(record.dOctets);

    // Flow duration
    if (systemUptime < MAX_FLOW_DURATION)
    {
      record.first = 0;
    }
    else
    {
      record.first = (systemUptime - (MIN_FLOW_DURATION + rand()) % MAX_FLOW_DURATION)*1000;
    }
    record.last = record.first + (rand() % MAX_FLOW_DURATION)*1000;
    record.first = htonl(record.first);
    record.last = htonl(record.last);

    record.srcPort = htons(generateRandomPortNumber());
    record.dstPort = htons(generateRandomPortNumber());

    record.pad = 0;

    // Transport protocol (TCP|UDP)
    record.prot = rand() % 2 ? IPPROTO_TCP : IPPROTO_UDP;
    record.tcpFlags = record.prot == IPPROTO_TCP ? generateRandomTCPFlags() : 0;

    // NIY
    record.tos = 0;
    record.srcAs = 0;
    record.dstAs = 0;
    record.srcMask = 0;
    record.dstMask = 0;

    record.drops = 0;

    memcpy(buffer + sizeof(struct netflowHeader) + flow*sizeof(struct netflowRecord), &record, sizeof(struct netflowRecord));
  }

  /* Setup header */
  header.version      = htons(5);
  header.count        = htons(numberOfFlows);

  header.sysUpTime    = htonl((currentTime - systemStartTime) * 1000); // Time since the program was run is used
  header.unixSecs     = htonl(currentTime);

  // Random amount of residual nanoseconds is generated for testing purposes
  header.unixNsecs    = htonl(rand() % (1000000000 - 1));

  header.flowSequence = htonl(totalFlowsSent);

  memcpy(buffer, &header, sizeof(struct netflowHeader));

  // returns size of generated pdu
  return sizeof(struct netflowHeader) + numberOfFlows*sizeof(struct netflowRecord);
}


