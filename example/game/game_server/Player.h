/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

#include <yarp/os/Semaphore.h>
#include "Thing.h"
#include "Login.h"

// a route to communicate with a player
class Replier {
public:
    virtual void send(const char *msg) = 0;
    virtual void broadcast(const char *msg) = 0;
};

class Player : public Replier {
public:
    Player() : mutex(1) {
  
    }

    // this is the main command processing function
    void apply(const char *command);

    // commands are broken into pieces and processed here
    void apply(int argc, const char *argv[]);

    // this sets a callback, to pass messages back to the user
    void setReplier(Replier *n_replier) {
        mutex.wait();
        replier = n_replier;
        mutex.post();
    }

    // anything that needs to be said is said via the replier callback
    virtual void send(const char *msg) {
        mutex.wait();
        if (replier!=NULL) {
            replier->send(msg);
        }
        mutex.post();
    }

    // anything that needs to be broadcast is done via the replier callback
    virtual void broadcast(const char *msg) {
        mutex.wait();
        if (replier!=NULL) {
            replier->broadcast(msg);
        }
        mutex.post();
    }

    // request a move for the player
    void move(int dx, int dy);

    // request a description of the player's surroundings
    void look();

    // request a shoot for the player
    void fire(int x, int y);

    // remove the player from the game
    void shutdown();

    // check whether player is present in the game
    bool isEmbodied() {
        return id.isValid();
    }
    void setEnergy(int e) {energy = e; };
    int  getEnergy() {return energy; };

    void setLife(int l) { login.getThing().setLife(l); };
    int  getLife() {return login.getThing().getLife(); };

    void setFirerange(int f) {firerange = f; };
    int  getFirerange() {return firerange; };

    void setName(const char *txt) { 
        login.getThing().setName(txt);
    }
    const char *getName() {
        return login.getThing().getName();
    }

private:
  
    Replier *replier;
    yarp::os::Semaphore mutex;

    ID id;
    Login login;
    int life;
    int energy;
    int firerange;
};

#endif
