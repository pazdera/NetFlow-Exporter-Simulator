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

#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "errors.h"
#include "netflow.h"
#include "nfgen.h"
#include "udp.h"
#include "hosts.h"
#include "binaryoutput.h"

/* Local port number */
#define SRC_PORT 10000

/* Default CLI arguments */
#define DEFAULT_ADDRESS "127.0.0.1"
#define DEFAULT_PORT 2055
#define DEFAULT_SEED time(NULL)

/* TODO A helpful help could be more useful. */
void usage(int exitCode)
{
  fprintf(stderr, "Usage: nfgen [-a address] [-p port] [-s seed] [-o path]\n");
  fprintf(stderr, "  -a collector addres (default %s)\n", DEFAULT_ADDRESS);
  fprintf(stderr, "  -p dest port (default %i)\n", DEFAULT_PORT);
  fprintf(stderr, "  -s generator seed (default randomized)\n");
  fprintf(stderr, "  -o output file\n");

  exit(exitCode);
}

struct cliArguments parseCliArguments(int argc, char **argv)
{
  error_t status = EOK;

  struct cliArguments arguments;
  convertAddress(DEFAULT_ADDRESS, &arguments.address);
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
      status = convertAddress(optarg, &arguments.address);
      if (status != EOK)
      {
        printError(status, "Invalid 'a' option argument");
        usage(EXIT_FAILURE);
      }    
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
        usage(EXIT_SUCCESS);
        break;
    default:
        printError(0, "Unknown option");
        usage(EXIT_FAILURE);
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

int main(int argc, char **argv)
{
  error_t status;
  
  struct cliArguments arguments = parseCliArguments(argc, argv);

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
    pduSize = makeRandomNetflowPacket(buffer, systemStartTime, numberOfFlows, totalFlowsSent);

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

