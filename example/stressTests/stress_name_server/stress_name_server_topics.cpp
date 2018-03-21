/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>

using namespace yarp::os;

#define N 6
#define R 5

class Count {
public:
    int counts[R];
    Semaphore mutex;

    Count() : mutex(1) {
        for (int i=0; i<R; i++) { counts[i] = 0; }
    }

    void count(int x) {
        mutex.wait();
        counts[x]++;
        mutex.post();
    }

    void show() {
        for (int i=0; i<R; i++) { 
            printf("Number %d: got %d (target %d)\n", i, counts[i], N*N);
        }
    }
};

class StressPublisher : public Thread {
public:
    int n;
    Port p;
    Count *counter;

    virtual bool threadInit() {
        p.open(ConstString("/test/pub/") + ConstString::toString(n));
        Network::connect(p.getName().c_str(),"topic://stressor");
        return true;
    }

    virtual void run() {
        for (int i=0; i<R; i++) {
            Bottle b;
            b.addString(p.getName());
            b.addInt(i);
            printf("Writing %s\n", b.toString().c_str());
            p.write(b);
            Time::delay(3);
        }
    }
};

class StressSubscriber : public Thread {
public:
    int n;
    Port p;
    Count *counter;

    virtual bool threadInit() {
        p.open(ConstString("/test/sub/") + ConstString::toString(n));
        Network::connect("topic://stressor",p.getName().c_str());
        return true;
    }

    virtual void onStop() {
        p.interrupt();
    }

    virtual void run() {
        while (!isStopping()) {
            Bottle b;
            if (p.read(b)) {
                printf("%s read %s\n", p.getName().c_str(),
                       b.toString().c_str());
                counter->count(b.get(1).asInt());
            }
        }
    }
};

int main(int argc, char *argv[]) {
    Network yarp;
    Count counter;
    StressPublisher pubs[N];
    StressSubscriber subs[N];
    for (int i=0; i<N; i++) {
        pubs[i].n = i;
        subs[i].n = i;
        pubs[i].counter = &counter;
        subs[i].counter = &counter;
        pubs[i].start();
        subs[i].start();
    }
    printf("Started.....\n");
    for (int i=0; i<N; i++) {
        pubs[i].stop();
    }
    printf("Stopping.\n");
    for (int i=0; i<N; i++) {
        subs[i].stop();
    }
    counter.show();

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            if (!NetworkBase::isConnected(pubs[i].p.getName(),
                                          subs[i].p.getName())) {
                printf(" *** failed to connect %s -> %s\n",
                       pubs[i].p.getName().c_str(),
                       subs[i].p.getName().c_str());
            }
        }
    }

    return 0;
}
