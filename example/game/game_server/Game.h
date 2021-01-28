/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#ifndef GAME_H
#define GAME_H

#include <yarp/os/Thread.h>

#include "Thing.h"

// ACE messes up main()
#ifdef main
#undef main
#endif

class Game : public yarp::os::Thread {
public:
    Game();

    virtual ~Game();

    void setMaze(const char *fname);

    void update();

    static Game& getGame();

    void setCell(ID x, ID y, ID val);

    void setTransient(ID x, ID y, int tag, double duration);
    int getTransient(ID x, ID y);

    ID getCell(ID x, ID y);

    Thing& newThing(bool putOnBoard = true);
    void killThing(ID x);

    bool isThing(ID id);
    Thing& getThing(ID id);

    virtual void run() { main(); }
    virtual void main();

    void save(bool force = false);
    void load();

    void wait();
    void post();

private:
    void *system_resource;
    void init();

};

#endif
