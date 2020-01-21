/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
 *
 * This example maintains a small help queue webserver.
 * After starting it, point a webbrowser at your local host and
 * click "help" or "read"
 *
 * You may need to run this as superuser in order to use port 80.
 *
 *   -- paulfitz
 *
 */

#include <yarp/os/all.h>
#include <yarp/dev/all.h>

#include <deque>
#include <string>
#include <cstdio>
#include <mutex>

using namespace yarp::os;
using namespace yarp::dev;
using namespace std;

class Entry {
public:
    string name;
};

class QueueManager : public DeviceResponder {
private:
    BufferedPort<Bottle> port;
    deque<Entry> q;
    std::mutex mutex;

    bool removeName(const char *name) {
        bool acted = false;
        for (deque<Entry>::iterator it=q.begin(); it!=q.end(); it++) {
            string nname = (*it).name;
            if (nname==name) {
                acted = true;
                q.erase(it);
                // iterators are now invalid
                break;
            }
        }
        if (acted) {
            // in case there are other copies, remove recursively
            removeName(name);
        }
        return acted;
    }

    bool removeName(int index) {
        bool acted = false;
        int at = 0;
        for (deque<Entry>::iterator it=q.begin(); it!=q.end(); it++) {
            string nname = (*it).name;
            if (at==index) {
                acted = true;
                q.erase(it);
                // iterators are now invalid
                break;
            }
            at++;
        }
        return acted;
    }

    void addQueue(Bottle& status) {
        status.add(Value::makeVocab("q"));
        for (deque<Entry>::iterator it=q.begin(); it!=q.end(); it++) {
            string name = (*it).name;
            status.addString(name.c_str());
        }
    }
public:
    QueueManager() : mutex() {
        attach(port);
        Contact c("/help", "tcp", "localhost", 80);
        port.open(c);
    }

    void updateHelp() {
        makeUsage();
        addUsage("[list]", "show the queue");
        for (deque<Entry>::iterator it=q.begin(); it!=q.end(); it++) {
            string name = (*it).name;
            string usage = "[del] ";
            usage += name;
            addUsage(usage.c_str(), "remove this name fram the queue");
        }
        addUsage("[add] $name", "add a name to the queue");
        addUsage("[del] $foo", "remove a name from the queue");
    }

    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply) {
        mutex.lock();
        switch (command.get(0).asVocab()) {
        case yarp::os::createVocab('a','d','d'):
            {
                string name = command.get(1).asString().c_str();
                if (name!="") {
                    removeName(name.c_str());
                    Entry entry;
                    entry.name = name;
                    q.push_back(entry);
                    reply.clear();
                    reply.add(Value::makeVocab("add"));
                    reply.addString(name.c_str());
                    addQueue(reply);
                }
            }
            break;
        case yarp::os::createVocab('d','e','l'):
            {
                if (command.get(1).isInt32()) {
                    int idx = command.get(1).asInt32();
                    bool acted = removeName(idx);
                    if (acted) {
                        reply.clear();
                        reply.add(Value::makeVocab("del"));
                        reply.addInt32(idx);
                    } else {
                        reply.clear();
                        reply.add(Value::makeVocab("no"));
                        reply.addInt32(idx);
                    }
                    addQueue(reply);
                } else {
                    string name = command.get(1).asString().c_str();
                    if (name!="") {
                        bool acted = removeName(name.c_str());
                        if (acted) {
                            reply.clear();
                            reply.add(Value::makeVocab("del"));
                            reply.addString(name.c_str());
                        } else {
                            reply.clear();
                            reply.add(Value::makeVocab("no"));
                            reply.addString(name.c_str());
                        }
                        addQueue(reply);
                    }
                }
            }
            break;
        case yarp::os::createVocab('l','i','s','t'):
            {
                reply.clear();
                addQueue(reply);
            }
            break;
        default:
            updateHelp();
            mutex.unlock();
            return DeviceResponder::respond(command,reply);
        }
        mutex.unlock();
        printf("%s\n", reply.toString().c_str());
        return true;
    }

    void update() {
        Bottle& status = port.prepare();
        mutex.lock();
        status.clear();
        addQueue(status);
        mutex.unlock();
        port.write();
    }
};

int main(int argc, char *argv[]) {
    Network yarp;
    QueueManager man;

    while (true) {
        Time::delay(5);
        man.update();
    }

    return 0;
}
