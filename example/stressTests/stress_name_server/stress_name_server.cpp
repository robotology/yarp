// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <yarp/os/all.h>

using namespace yarp::os;

class StressPublisher : public Thread {
public:
    int n;
    Port p;

    virtual bool threadInit() {
        p.open(ConstString("/test/pub/") + ConstString::toString(n));
        Network::connect(p.getName().c_str(),"topic://stressor");
        return true;
    }

    virtual void run() {
        for (int i=0; i<10; i++) {
            Bottle b;
            b.addString(p.getName());
            b.addInt(i);
            printf("Writing %s\n", b.toString().c_str());
            p.write(b);
            Time::delay(1);
        }
    }
};

class StressSubscriber : public Thread {
public:
    int n;
    Port p;

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
            }
        }
    }
};

#define N 15

int main(int argc, char *argv[]) {
    Network yarp;
    StressPublisher pubs[N];
    StressSubscriber subs[N];
    for (int i=0; i<N; i++) {
        pubs[i].n = i;
        subs[i].n = i;
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
    return 0;
}
