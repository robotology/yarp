/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdlib.h>

#include <mutex>
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
    Player() : mutex() {

    }

    // this is the main command processing function
    void apply(const char *command);

    // commands are broken into pieces and processed here
    void apply(int argc, const char *argv[]);

    // this sets a callback, to pass messages back to the user
    void setReplier(Replier *n_replier) {
        mutex.lock();
        replier = n_replier;
        mutex.unlock();
    }

    // anything that needs to be said is said via the replier callback
    virtual void send(const char *msg) {
        mutex.lock();
        if (replier!=NULL) {
            replier->send(msg);
        }
        mutex.unlock();
    }

    // anything that needs to be broadcast is done via the replier callback
    virtual void broadcast(const char *msg) {
        mutex.lock();
        if (replier!=NULL) {
            replier->broadcast(msg);
        }
        mutex.unlock();
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
    std::mutex mutex;

    ID id;
    Login login;
    int life;
    int energy;
    int firerange;
};

#endif
