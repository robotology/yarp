/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <mutex>

#include <yarp/os/all.h>
#include <yarp/Logger.h>
using namespace yarp::os;
using namespace yarp;

#include <ace/OS.h>

#include "simio.h"
#include "keyboard.h"
using namespace yarp;

//#define real_printf printf
//#define printf cprintf

String pad(const String& src, int len = 70) {
    String result = src;
    int ct = len-src.length();
    for (int i=0; i<ct; i++) {
        result += " ";
    }
    return result;
}

std::mutex broadcastMutex();
String broadcast = "";

class BroadcastHandler : public TypedReaderCallback<Bottle> {
public:
    virtual void onRead(Bottle& bot) {
        broadcastMutex.lock();
        broadcast = bot.toString().c_str();
        broadcastMutex.unlock();
    }
} handler;



class UpdateThread : public Thread {
public:
    Port p;
    String name;
    PortReaderBuffer<Bottle> reader;
    std::mutex mutex;

    UpdateThread() : mutex() {
    }

    void setName(const char *name) {
        this->name = name;
    }

    void connect() {
        reader.attach(p);
        reader.useCallback(handler);

        if (name[0]!='.') {
            name = String("/player/") + name;
        }
        p.open(name.c_str());

        printf("Connecting...\n");

        Logger::get().setVerbosity(-1);

        // we'll be sending messages to the game (and getting responses)
        Network::connect(p.getName(),"/game");

        // there are occasional messages broadcast from the game to us
        Network::connect("/game",p.getName(),"mcast");

    }

    void show() {
        int xx = 0;
        int yy = 1;
        mutex.lock();
        Bottle send("look");
        Property prop;
        p.write(send,prop);
        gotoxy(0,0);
        Bottle& map = prop.findGroup("look").findGroup("map");
        broadcastMutex.lock();
        String prep = getPreparation().c_str();
        if (prep.length()>0) {
            long int t = (long int)Time::now();
            xx = prep.length();
            if (t%2==0) {
                prep = prep + "*";
            }
        }
        cprintf("%s\n%s\n%s\n",
                pad("").c_str(),
                pad(prep).c_str(),
                pad(broadcast).c_str());
        broadcastMutex.unlock();
        int i;
        for (i=1; i<map.size(); i++) {
            cprintf("  %s\n", map.get(i).asString().c_str());
        }
        cprintf("\n");
        Bottle& players = prop.findGroup("look").findGroup("players");
        for (i=1; i<players.size(); i++) {
            Bottle *player = players.get(i).asList();
            if (player!=NULL) {
                Bottle& location = player->findGroup("location");
                Value& life = player->find("life");
                char buf[256];
                std::string playerName = player->get(0).asString();
                if (strlen(playerName.c_str())<40) {
                    sprintf(buf,"PLAYER %s is at (%d,%d) with lifeforce %d",
                                    playerName.c_str(),
                                    location.get(1).asInt32(),
                                    location.get(2).asInt32(),
                                    life.asInt32());
                    cprintf("%s\n", pad(String(buf)).c_str());
                }
            }
        }
        for (int j=players.size(); j<=5; j++) {
            cprintf("%s\n", pad(String("")).c_str());
        }

        gotoxy(xx,yy);
        mutex.unlock();
    }

    void apply(const String& str) {
        Bottle send, recv;
        send.fromString(str.c_str());
        mutex.lock();
        p.write(send,recv);
        if (recv.get(0).asString()=="error") {
            cprintf("PROBLEM:\n");
            cprintf("  request: %s\n", send.toString().c_str());
            cprintf("  response: %s\n", recv.toString().c_str());
            refresh();
            Time::delay(2);
        }
        mutex.unlock();
        //show();
    }

    virtual void run() {
        while (!isStopping()) {
            Time::delay(0.25);
            show();
        }
        mutex.lock();
        p.close();
        mutex.unlock();
    }

} update_thread;


void stop(int x) {
    clrscr();
    cprintf("Stopping...\n");
    autorefresh();
    update_thread.stop();
    Time::delay(0.5);
    deinitconio();
    Time::delay(0.5);
    exit(0);
}



void mainloop() {
    update_thread.start();

    bool done = false;
    while (!done) {
        String str = getCommand();
        if (str!="") {
            if (str=="quit") {
                done = true;
                break;
            }
            update_thread.apply(str);
        }
    }

    update_thread.stop();
    deinitconio();
}




int main(int argc, char *argv[]) {
    Network yarp;

    const char *name = "...";
    if (argc>=2) {
        name = argv[1];
    }
    update_thread.setName(name);

    update_thread.connect();

    // switch to graphic mode
    initconio();
    setautorefresh(1);
    clrscr();

#ifndef WIN32
    signal(SIGKILL,stop);
    signal(SIGINT,stop);
    signal(SIGTERM,stop);
    signal(SIGPIPE,stop);
#endif

    mainloop();

    return 0;
}
