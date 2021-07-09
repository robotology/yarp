/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This example maintains a small help queue webserver.
 * After starting it, point a webbrowser at your local host and
 * click "help" or "read"
 *
 * You may need to run this as superuser in order to use port 80.
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <yarp/dev/DeviceDriver.h>

#include <cstdio>
#include <deque>
#include <mutex>
#include <string>

using namespace yarp::os;
using namespace yarp::dev;

constexpr double loop_delay = 5.0;

class Entry
{
public:
    std::string name;
};

class QueueManager : public DeviceResponder
{
private:
    BufferedPort<Bottle> port;
    std::deque<Entry> q;
    std::mutex mutex;

    bool removeName(const char* name)
    {
        bool acted = false;
        for (auto it = q.begin(); it != q.end(); it++) {
            std::string nname = (*it).name;
            if (nname == name) {
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

    bool removeName(int index)
    {
        bool acted = false;
        int at = 0;
        for (std::deque<Entry>::iterator it = q.begin(); it != q.end(); it++) {
            std::string nname = (*it).name;
            if (at == index) {
                acted = true;
                q.erase(it);
                // iterators are now invalid
                break;
            }
            at++;
        }
        return acted;
    }

    void addQueue(Bottle& status)
    {
        status.addVocab32('q');
        for (const auto& entry : q) {
            std::string name = entry.name;
            status.addString(name.c_str());
        }
    }

public:
    QueueManager()
    {
        attach(port);
        Contact c("/help", "tcp", "localhost", 80);
        port.open(c);
    }

    void updateHelp()
    {
        makeUsage();
        addUsage("[list]", "show the queue");
        for (const auto& entry : q) {
            std::string name = entry.name;
            std::string usage = "[del] ";
            usage += name;
            addUsage(usage.c_str(), "remove this name fram the queue");
        }
        addUsage("[add] $name", "add a name to the queue");
        addUsage("[del] $foo", "remove a name from the queue");
    }

    bool respond(const yarp::os::Bottle& command,
                 yarp::os::Bottle& reply) override
    {
        mutex.lock();
        switch (command.get(0).asVocab32()) {
        case yarp::os::createVocab32('a', 'd', 'd'): {
            std::string name = command.get(1).asString();
            if (!name.empty()) {
                removeName(name.c_str());
                Entry entry;
                entry.name = name;
                q.push_back(entry);
                reply.clear();
                reply.addVocab32("add");
                reply.addString(name.c_str());
                addQueue(reply);
            }
        } break;
        case yarp::os::createVocab32('d', 'e', 'l'): {
            if (command.get(1).isInt32()) {
                int idx = command.get(1).asInt32();
                bool acted = removeName(idx);
                if (acted) {
                    reply.clear();
                    reply.addVocab32("del");
                    reply.addInt32(idx);
                } else {
                    reply.clear();
                    reply.addVocab32("no");
                    reply.addInt32(idx);
                }
                addQueue(reply);
            } else {
                std::string name = command.get(1).asString();
                if (!name.empty()) {
                    bool acted = removeName(name.c_str());
                    if (acted) {
                        reply.clear();
                        reply.addVocab32("del");
                        reply.addString(name.c_str());
                    } else {
                        reply.clear();
                        reply.addVocab32("no");
                        reply.addString(name.c_str());
                    }
                    addQueue(reply);
                }
            }
        } break;
        case yarp::os::createVocab32('l', 'i', 's', 't'): {
            reply.clear();
            addQueue(reply);
        } break;
        default:
            updateHelp();
            mutex.unlock();
            return DeviceResponder::respond(command, reply);
        }
        mutex.unlock();
        printf("%s\n", reply.toString().c_str());
        return true;
    }

    void update()
    {
        Bottle& status = port.prepare();
        mutex.lock();
        status.clear();
        addQueue(status);
        mutex.unlock();
        port.write();
    }
};

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Network yarp;
    QueueManager man;

    while (true) {
        Time::delay(loop_delay);
        man.update();
    }

    return 0;
}
