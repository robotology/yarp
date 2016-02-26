/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>

#include "Thread.h"
#include "Time.h"

class Thread1 : public Thread {
public:
    virtual void run() {
        while (!isStopping()) {
            printf("Hello\n");
            Time::delay(1);
        }
    }
};


class Thread2 : public Thread {
public:
    virtual void run() {
        Time::delay(0.5);
        while (!isStopping()) {
            printf("World\n");
            Time::delay(1);
        }
    }
};


int main() {
    Thread1 t1;
    Thread2 t2;
    printf("starting...\n");
    t1.start();
    t2.start();
    printf("started\n");
    Time::delay(3);
    printf("stopping...\n");
    t1.stop();
    t2.stop();
    printf("stopped\n");
    return 0;
}

