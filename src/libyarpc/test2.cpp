/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include "yarp.h"

int testResult = 0;

int testWrite(yarpWriterPtr connection, void *ptr) {
    printf("Writing an integer\n");
    return yarpWriterAppendInt32(connection,15);
}

int testRead(yarpReaderPtr connection, void *ptr) {
    printf("Reading an integer\n");
    return yarpReaderExpectInt32(connection,&testResult);
}

int main(int argc, char *argv[]) {
    int result = 0;

    printf("This test creates two ports and writes from one to the other.\n");
    printf("Make sure no other YARP programs are running.\n");
    printf("(or else remove the yarpNetworkSetLocalMode line)\n");

    yarpNetworkPtr network = yarpNetworkCreate();
    if (network==NULL) return -1;

    // assume no other YARP programs are running, including the name server
    result = yarpNetworkSetLocalMode(network,1);
    if (result<0) return -1;

    yarpPortPtr port1 = yarpPortCreateOpen(network,"/test1");
    if (port1==NULL) return -1;

    yarpPortPtr port2 = yarpPortCreateOpen(network,"/test2");
    if (port2==NULL) return -1;

    result = yarpNetworkConnect(network,"/test1","/test2",NULL);
    if (result<0) return -1;

    result = yarpPortEnableBackgroundWrite(port1,1);
    if (result<0) return -1;

    yarpPortableCallbacks wcallbacks, rcallbacks;
    yarpPortableCallbacksInit(&wcallbacks);
    yarpPortableCallbacksInit(&rcallbacks);
    wcallbacks.write = testWrite;
    rcallbacks.read = testRead;
    yarpPortable writer, reader;
    yarpPortableInit(&writer,&wcallbacks);
    yarpPortableInit(&reader,&rcallbacks);

    printf("Writing (in background)...\n");

    result = yarpPortWrite(port1,&writer);
    if (result<0) return -1;

    printf("Reading...\n");

    result = yarpPortRead(port2,&reader,0);
    if (result<0) return -1;

    printf("After read, received %d\n", testResult);
    if (testResult==15) {
        printf("Correct!\n");
    } else {
        printf("That's not right, something failed.\n");
    }

    result = yarpPortClose(port1);
    if (result<0) return -1;

    result = yarpPortClose(port2);
    if (result<0) return -1;

    yarpPortableFini(&writer);
    yarpPortableFini(&reader);

    yarpPortFree(port1);
    port1 = NULL;

    yarpPortFree(port2);
    port2 = NULL;

    yarpNetworkFree(network);
    network = NULL;

    return 0;
}
