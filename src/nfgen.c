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
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

#include <sys/stat.h>

#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "netflow.h"
#include "nfgen.h"

#define MIN_FLOW_DURATION 1
#define MAX_FLOW_DURATION 60

/* Local port number */
#define SRC_PORT 10000

/* Default CLI arguments */
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 2055
#define DEFAULT_SEED 1

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
size_t makeNetflowPacket(char *buffer, time_t systemStartTime, unsigned int numberOfFlows, unsigned int totalFlowsSent)
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

FILE* openOutputFile(char* path)
{
    FILE* file = fopen(path, "w+b");

    if (file == NULL)
    {
        perror("Unable to write to output file.");
        exit(EXIT_FAILURE);
    }

    return file;
}

void writeToOutputFile(FILE* file, void* datagram, size_t datagramSize)
{
    fwrite(datagram, sizeof(char), datagramSize, file);
    fflush(file);

    if (ferror(file))
    {
        perror("Cannot write into output file.");
        exit(EXIT_FAILURE);
    }
}

void closeOutputFile(FILE* file)
{
    fclose(file);
}

struct cliArguments parseCliArguments(int argc, char **argv)
{
  struct cliArguments arguments;
  arguments.address    = convertAddress(DEFAULT_ADDRESS);
  arguments.port       = DEFAULT_PORT;
  arguments.seed       = DEFAULT_SEED;
  arguments.outputFile = NULL;
  arguments.help       = 0;

  int option;
  /* TODO Some validation would be nice ... */
  while ((option = getopt(argc, argv, "a:p:s:o:h")) != -1)
  {
    switch (option)
    {
    case 'a':
      arguments.address = convertAddress(optarg);
      break;
    case 'p':
      arguments.port = atoi(optarg);
      break;
    case 's':
      arguments.seed = atoi(optarg);
      break;
    case 'o':
      arguments.outputFile = (char*) malloc((strlen(optarg) + 1)*sizeof(char));
      strcpy(arguments.outputFile, optarg);
      break;
    case 'h':
      arguments.help = 1;
      break;
    default: /* '?' */
      arguments.help = 1;
    }
  }

  return arguments;
}

void freeCliArguments(struct cliArguments arguments)
{
    if (arguments.outputFile != NULL)
    {
        free(arguments.outputFile);
    }
}

/* TODO A helpful help could be more useful. */
void usage(char **argv)
{
  fprintf(stderr, "Usage: %s [-a address] [-p port] [-s seed] [-o path]\n", argv[0]);
  fprintf(stderr, "  -a collector addres (default %s)\n", DEFAULT_ADDRESS);
  fprintf(stderr, "  -p dest port (default %i)\n", DEFAULT_PORT);
  fprintf(stderr, "  -s generator seed (default %i)\n", DEFAULT_SEED);
  fprintf(stderr, "  -o output file\n");
}

int main(int argc, char **argv)
{
  struct cliArguments arguments = parseCliArguments(argc, argv);

  if (arguments.help)
  {
    usage(argv);
    return EXIT_SUCCESS;
  }

  time_t systemStartTime = time(0);
  unsigned int numberOfFlows = 0;
  unsigned int totalFlowsSent = 0;

  /* Initialize generator */
  srand(arguments.seed);

  char buffer[MAX_NETFLOW_PDU_SIZE];
  memset(buffer, 0, MAX_NETFLOW_PDU_SIZE);
  size_t pduSize;

  int udpSocket = udpInitialize();
  FILE* outputFile = (arguments.outputFile != NULL) ? openOutputFile(arguments.outputFile) : NULL;

  while(1)
  {
    numberOfFlows = (1 + rand()) % MAX_NETFLOW_RECORDS;
    totalFlowsSent += numberOfFlows;
    pduSize = makeNetflowPacket(buffer, systemStartTime, numberOfFlows, totalFlowsSent);

    /* FIXME Some more information would be nice */
    if (udpSend(udpSocket, arguments.address, arguments.port, buffer, pduSize) == pduSize)
    {
      if (arguments.outputFile != NULL)
      {
        writeToOutputFile(outputFile, buffer, pduSize);
      }

      fprintf(stderr, "Packet of size %i with %i flows sent.\n", pduSize, numberOfFlows);
    }
    else
    {
      fprintf(stderr, "Sending failed.\n");
    }

    /* TODO Interval ought to be more versatile */
    sleep(rand() % 3);
  }

  if (outputFile != NULL)
  {
    closeOutputFile(outputFile);
  }

  udpClose(udpSocket);
  freeCliArguments(arguments);

  return EXIT_SUCCESS;
}
