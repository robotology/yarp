/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/**
 *
 * This is a YARP wrapper around the Nova game used in Giorgio Metta's
 * operating systems class at the University of Genoa, Italy
 *   http://www.liralab.it/teaching/OS/
 *
 * -paulfitz
 *
 */


#include <stdio.h>

#include "Game.h"
#include "Player.h"

#include <yarp/os/all.h>
#include <string>

using namespace yarp::os;

typedef std::string String;


Semaphore clientMutex(1);
static int clientCount = 0;


class ClientService : public PortReader, public Replier {
public:
    int id;
    bool loggedOn;
    Player player;
    String result;
    ConnectionWriter *writer;
    Replier& broadcaster;

    ClientService(Replier& broadcaster) : broadcaster(broadcaster) {
        loggedOn=false;
        clientMutex.wait();
        clientCount++;
        printf("Connection created, client #%d\n", clientCount);
        id = clientCount;
        clientMutex.post();
        player.setReplier(this);
        writer = NULL;
    }

    virtual ~ClientService() {
        player.setReplier(NULL);
        player.shutdown();
        clientMutex.wait();
        printf("Connection shut down for %d\n", id);
        clientCount--;
        clientMutex.post();
    }

    virtual bool read(ConnectionReader& connection) {
        result = "";
        printf("Reading something from <%s>\n", 
               connection.getRemoteContact().getName().c_str());
        if (!loggedOn) {
            printf("Completing login...\n");
            String cmd = "connect ";
            cmd += connection.getRemoteContact().getName().c_str();
            player.apply(cmd.c_str());
            loggedOn = true;
        }
        Bottle receive;
        receive.read(connection);
        writer = connection.getWriter();
        if (writer!=NULL) {
            // just send the same thing back
            ConstString str = receive.get(0).asString();
            String ask = str.c_str();
            for (int i=1; i<receive.size(); i++) {
                ask += " ";
                ask += receive.get(i).asString().c_str();
            }
            printf("COMMAND %s\n", ask.c_str());
            player.apply(ask.c_str());
            flush();
        }
        return true;
    }


    void flush() {
        if (result.length()>0 && loggedOn) {
            //if (reply.size()>0) {
            if (writer->isTextMode()) {
                writer->appendString(result.c_str());
            } else {
                Bottle reply;
                reply.fromString(result.c_str());
                //printf("writing... [%s]\n", reply.toString().c_str());
                reply.write(*writer);
            }
        }
        result = "";
    }

    virtual void send(const char *msg) {
        if (msg==NULL) {
            flush();
            return;
        }
        //printf("asked to send %s\n", msg);
        if (result!="") {
            result += "\r\n";
        }
        result += msg;
        if (msg[0]!='\0') {
            int ct = 0;
            for (unsigned int i=0; i<ACE_OS::strlen(msg); i++) {
                if (msg[i] == '(') {
                    ct++;
                }
                if (msg[i] == ')') {
                    ct--;
                }
            }
            if (msg[0]!=' ' && ct<=0) {
                flush();
            }
        }
    }

    virtual void broadcast(const char *msg) {
        broadcaster.broadcast(msg);
    }
};



class ClientFactory : public PortReaderCreator, public Replier {
public:

    Port man;

    ClientFactory() {
        man.setReaderCreator(*this);
        man.open(Contact("/game", "...", "...", 8080));
        Contact where = man.where();
        printf("Game reachable at ip %s port %d (registered with yarp as %s)\n", 
               where.getHost().c_str(),
               where.getPort(),
               where.getName().c_str());
    }

    virtual PortReader *create() {
        return new ClientService(*this);
    }

    virtual void send(const char *msg) {
    }


    virtual void broadcast(const char *msg) {
        printf("asked to broadcast %s\n", msg);
        Bottle bot(msg);
        man.write(bot);
    }
};



int main() {
    Game::getGame().update();

    Network yarp;
    //Network::setLocalMode(true);
    ClientFactory factory;
    Game::getGame().main();
  
    return 0;
}

