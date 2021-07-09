/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <assert.h>

#include <mutex>
#include <yarp/os/Time.h>

#include "Thing.h"
#include "Game.h"

#include "Matrix.h"
#include "DMatrix.h"
#include "Things.h"

using namespace yarp::os;

// last minute need for random number...
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <ace/OS.h>
static double ranf () {
    return double (ACE_OS::rand ()) / double (RAND_MAX);
}



class GameHelper {
public:
    Matrix game_matrix;
    DMatrix transient_matrix;
    Things game_things;
    std::mutex game_mutex;

    GameHelper() : game_mutex() {
    }
};

#define SYS(x) (*((GameHelper*)(x)))

Game *the_game = NULL;


Game::Game() {
    system_resource = new GameHelper();
    init();
}

Game::~Game() {
    delete &SYS(system_resource);
}

void Game::init() {
    setMaze("maze.txt");
    start();
}


void Game::setMaze(const char *fname) {
    Matrix& m = SYS(system_resource).game_matrix;
    ifstream fin(fname);
    if (!fin.good()) {
        printf("Cannot find maze file %s\n", fname);
        printf("Continuing with blank maze\n");
        return;
    }
    int y = 0;
    int ct = 0;
    printf("Initializing maze...\n");
    while (fin.good() && !fin.eof()) {
        char buf[2000];
        fin.getline(buf,sizeof(buf));
        for (unsigned int x=0; x<ACE_OS::strlen(buf); x++) {
            char ch = buf[x];
            ID v = 0;
            if (ch=='+' || ch=='-' || ch=='|'||ch=='#') { v = 1; }
            if (ch>='0'&&ch<='9') {
                v = SYS(system_resource).game_things.create();
                SYS(system_resource).game_things.getThing(v).set(x,y,v);
            }
            if (v.asInt32()!=0) {
                m.set(x,y,v);
                ct++;
            }
        }
        y++;
    }
    printf("Maze initialized (%d blocks)\n", ct);
}

void Game::update() {
    SYS(system_resource).game_things.update();
}

Game& Game::getGame() {
    //return the_game;
    if (the_game==NULL) { the_game = new Game; }
    return *the_game;
}

Thing& Game::getThing(ID id) {
    Thing& thing = SYS(system_resource).game_things.getThing(id);
    return thing;
}

bool Game::isThing(ID id) {
    return SYS(system_resource).game_things.isThing(id);
}

Thing& Game::newThing(bool putOnBoard) {
    ID id = -1;
    for (int i=100; i<400; i++) {
        if (!isThing(ID(i))) {
            id = i;
            break;
        }
    }

    if(putOnBoard) {

        if (id.asInt32()!=-1) {
            ID xx = -1;
            ID yy = -1;

            for (int i=0; i<100; i++) {
                ID x = (int)(ranf()*20);
                ID y = (int)(ranf()*20);
                if (SYS(system_resource).game_matrix.get(x,y).asInt32()==0) {
                    xx = x;
                    yy = y;
                }
            }

            if (xx==-1) {
                for (int r=1; r<20 && xx.asInt32()==-1; r++) {
                    for (int n=0; n<r && xx.asInt32()==-1; n++) {
                        ID x = r-n;
                        ID y = n;
                        if (SYS(system_resource).game_matrix.get(x,y).asInt32()==0) {
                            xx = x;
                            yy = y;
                        }
                    }
                }
            }
            if (xx.asInt32()!=-1) {
                printf("Make new at %ld %ld\n", xx.asInt32(), yy.asInt32());
                SYS(system_resource).game_things.create(id);
                SYS(system_resource).game_things.getThing(id).set(xx,yy,id);
                SYS(system_resource).game_matrix.set(xx,yy,id);
            }
        }

    } // putOnBoard

    else {
        SYS(system_resource).game_things.create(id);
    }

    if (id.asInt32()==-1) {
        return Thing::NOTHING;
    }
    return getThing(id);
}


void Game::killThing(ID x) {
    wait();
    Thing& thing = SYS(system_resource).game_things.getThing(x);
    SYS(system_resource).game_matrix.set(thing.getX(),thing.getY(),0);
    SYS(system_resource).game_things.destroy(x);
    post();
}



ID Game::getCell(ID x, ID y) {
    return SYS(system_resource).game_matrix.get(x,y);
}

void Game::setCell(ID x, ID y, ID val) {
    SYS(system_resource).game_matrix.set(x,y,val);
}


void Game::main() {
    while (1) {
        wait();
        update();
        post();
        yarp::os::Time::delay(0.1);
    }
}


void Game::save(bool force) {
    // incremental or full save
    // for now, always full

    wait();
    SYS(system_resource).game_matrix.save();
    post();
}

void Game::load() {
}

void Game::wait() {
    SYS(system_resource).game_mutex.lock();
}

void Game::post() {
    SYS(system_resource).game_mutex.unlock();
}


void Game::setTransient(ID x, ID y, int tag, double duration) {
    SYS(system_resource).transient_matrix.set(x,y,Time::now()+duration);
}

int Game::getTransient(ID x, ID y) {
    return SYS(system_resource).transient_matrix.get(x,y)>Time::now();
}


class GameDown {
public:
    GameDown() {};
    ~GameDown() {
        if (the_game!=NULL) {
            delete the_game;
        }
    }
} game_down;
