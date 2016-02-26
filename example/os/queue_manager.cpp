/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#include <stdio.h>
using namespace yarp::os;
using namespace yarp::dev;

#include <deque>
#include <string>
using namespace std;

class Entry {
public:
    string name;
};

class QueueManager : public DeviceResponder {
private:
    BufferedPort<Bottle> port;
    deque<Entry> q;
    Semaphore mutex;

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
    QueueManager() : mutex(1) {
        attach(port);
        Contact c = Contact::byName("/help");
        c = c.addSocket("tcp","localhost",80);
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
        mutex.wait();
        switch (command.get(0).asVocab()) {
        case VOCAB3('a','d','d'):
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
        case VOCAB3('d','e','l'):
            {
                if (command.get(1).isInt()) {
                    int idx = command.get(1).asInt();
                    bool acted = removeName(idx);
                    if (acted) {
                        reply.clear();
                        reply.add(Value::makeVocab("del"));
                        reply.addInt(idx);
                    } else {
                        reply.clear();
                        reply.add(Value::makeVocab("no"));
                        reply.addInt(idx);
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
        case VOCAB4('l','i','s','t'):
            {
                reply.clear();
                addQueue(reply);
            }
            break;
        default:
            updateHelp();
            mutex.post();
            return DeviceResponder::respond(command,reply);
        }
        mutex.post();
        printf("%s\n", reply.toString().c_str());
        return true;
    }

    void update() {
        Bottle& status = port.prepare();
        mutex.wait();
        status.clear();
        addQueue(status);
        mutex.post();
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
