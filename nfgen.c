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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/stat.h>
// #include <sys/types.h>


#include <string.h>
#include <time.h>

#define TCP_PROTO 6
#define UDP_PROTO 17

#define MIN_FLOW_DURATION 1
#define MAX_FLOW_DURATION 60

#define SRC_PORT 10000
#define DEST_PORT 2055

#define PRG_SEED 5

#define NETFLOW_HEADER_SIZE 24
#define NETFLOW_RECORD_SIZE 48

#define NUMBER_OF_ADDRESSES 4
in_addr_t addresses[NUMBER_OF_ADDRESSES] =
{
//   "127.0.0.1",
//   "192.168.1.100",
//   "192.168.1.101",
//   "192.168.1.102"
  0x7F000001, /* 127.0.0.1 */
  0xC0A80164, /* 192.168.1.100 */
  0xC0A80165, /* 192.168.1.101 */
  0xC0A80166  /* 192.168.1.102 */
};



// in_addr_t convertAddress(char *addressInDotNotation)
// {
//   in_addr structureForConversionResult;
// 
//   if (inet_aton(addressInDotNotation, &structureForConversionResult) != 1)
//   {
//     perror("Address conversion failed.");
//   }
// 
//   return structureForConversionResult.s_addr;
// }

in_addr_t generateRandomAddress()
{
  /* This could be more sophisticated */
  return addresses[(1 + rand()) % NUMBER_OF_ADDRESSES];
}

in_port_t generateRandomPortNumber()
{
  return rand() % 65536;
}

char generateRandomTCPFlags()
{
  return rand() % 255;
}

struct
{
  uint16_t version;       /* 5 */
  uint16_t count;         /* The number of records in the PDU */
  uint32_t sysUpTime;     /* Current time in millisecs since router booted */
  uint32_t unixSecs;      /* Current seconds since 0000 UTC 1970 */
  uint32_t unixNsecs;     /* Residual nanoseconds since 0000 UTC 1970 */
  uint32_t flowSequence;  /* Seq counter of total flows seen */
  uint8_t  engineType;    /* Type of flow switching engine (RP,VIP,etc.) */
  uint8_t  engineId;      /* Slot number of the flow switching engine */
  uint16_t reserved;
} header;

struct
{
  uint32_t srcAddr;     /* Source IP Address */
  uint32_t dstAddr;     /* Destination IP Address */
  uint32_t nextHop;     /* Next hop router's IP Address */
  uint16_t input;       /* Input interface index */
  uint16_t output;      /* Output interface index */
  uint32_t dPkts;       /* Packets sent in Duration */
  uint32_t dOctets;     /* Octets sent in Duration. */
  uint32_t first;       /* SysUptime at start of flow */
  uint32_t last;        /* and of last packet of flow */
  uint16_t srcPort;     /* TCP/UDP source port number or equivalent */
  uint16_t dstPort;     /* TCP/UDP destination port number or equiv */
  uint8_t  pad;
  uint8_t  tcpFlags;    /* Cumulative OR of tcp flags */
  uint8_t  prot;        /* IP protocol, e.g., 6=TCP, 17=UDP, ... */
  uint8_t  tos;         /* IP Type-of-Service */
  uint16_t srcAs;       /* originating AS of source address */
  uint16_t dstAs;       /* originating AS of destination address */
  uint8_t  srcMask;     /* source address prefix mask bits */
  uint8_t  dstMask;     /* destination address prefix mask bits */
  uint16_t drops;
} record;

size_t makeNetflowPacket(char *buffer, int numberOfFlows, time_t systemStartTime)
{
  time_t currentTime = time(0);

  for (int flow = 0;flow < numberOfFlows; flow++)
  {
    // Setup record
    record.srcAddr = htonl(generateRandomAddress());
    record.dstAddr = htonl(generateRandomAddress());
    record.nextHop = 0;
    record.input = 0;
    record.output = 0;
    record.dPkts = htonl(100);
    record.dOctets = htonl(120*8);

    record.first = (currentTime - systemStartTime - (MIN_FLOW_DURATION + rand()) % MAX_FLOW_DURATION)*1000;
    record.last = record.first + (rand() % MAX_FLOW_DURATION)*1000;
    record.first = htonl(record.first);
    record.last = htonl(record.last);

    record.srcPort = htons(generateRandomPortNumber());
    record.dstPort = htons(generateRandomPortNumber());
    record.pad = 0;
    record.prot = rand() % 2 ? TCP_PROTO : UDP_PROTO;
    record.tcpFlags = record.prot == TCP_PROTO ? generateRandomTCPFlags() : 0;
    record.tos = 0;
    record.srcAs = 0;
    record.dstAs = 0;
    record.srcMask = 0;
    record.dstMask = 0;
    record.drops = 0;

    memcpy(buffer + NETFLOW_HEADER_SIZE + flow*NETFLOW_RECORD_SIZE, &record, NETFLOW_RECORD_SIZE);
  }

  // Setup header
  header.version      = 5;
  header.count        = numberOfFlows;
  header.sysUpTime    = htonl((currentTime - systemStartTime) * 1000);
  header.unixSecs     = htonl(currentTime);
  header.unixNsecs    = htonl(rand() % (1000000000 - 1)); // nanoseconds
  header.flowSequence = 0;

  memcpy(buffer, &header, NETFLOW_HEADER_SIZE);

  return NETFLOW_HEADER_SIZE + numberOfFlows*NETFLOW_RECORD_SIZE;
}

int udpSend(in_addr_t addr, in_port_t port, void *buff, size_t nbytes)
{
  int udtSocketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);

  if (udtSocketFileDescriptor <= 0)
  {
    perror("Unable to create socket.");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in socketAddress;
  memset(&socketAddress, 0, sizeof(socketAddress));
  socketAddress.sin_family = AF_INET;
  socketAddress.sin_addr.s_addr = htonl(0);
  socketAddress.sin_port = htons(SRC_PORT);

  if (bind(udtSocketFileDescriptor, (const struct sockaddr *) &socketAddress, sizeof(socketAddress)) == -1)
  {
    perror("Unable to bind.");
    exit(EXIT_FAILURE);
  }

  // Now we're ready to go.
  struct stat info;
  if (fstat(udtSocketFileDescriptor, &info) != 0)
  {
    perror("Descriptor is invalid.");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in sa;
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = htonl(addr);
  sa.sin_port = htons(port);
  ssize_t nsend = sendto(udtSocketFileDescriptor, buff, nbytes, 0, (const struct sockaddr *) &sa, sizeof(sa));

  close(udtSocketFileDescriptor);

  return nsend;
}

int main(int argc, char **argv)
{
  in_addr_t remoteAddress = 0x7f000001; // 127.0.0.1
  in_port_t remotePort = DEST_PORT;

  time_t systemStartTime = time(0);
  unsigned int flowsGenerated = 0;

  int seed = PRG_SEED;
  srand(seed);

  char buffer[1480];
  memset(buffer, 0, 1480);
  size_t pduSize = 0;

  while(1)
  {
    flowsGenerated = (1 + rand()) % 30;
    pduSize = makeNetflowPacket(buffer, flowsGenerated, systemStartTime);

    fprintf(stderr, "%i\n", udpSend(remoteAddress, remotePort, buffer, pduSize));

    sleep(rand() % 3);
  }

  return 0;
}