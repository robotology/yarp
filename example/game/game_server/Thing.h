/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef THING_H
#define THING_H

#include <cmath>
#include <string.h>

#include <ace/OS.h>

#include "ID.h"


class Thing {
private:
    ID id;
    ID x, y;
    int dx, dy;
    int lifetime;
    int life;
    char name[256];

public:
    Thing() { id = -1; x = y = 0; dx = dy = 0; lifetime = -1; life = 0; }

    static Thing NOTHING;

    void set(ID n_x, ID n_y, ID n_id = -1);

    void setID(ID n_id) { id = n_id; }


    void setMove(int ndx, int ndy) {
        dx = ndx;
        dy = ndy;
    }

    ID getID() { return id; }
    ID getX() { return x; }
    ID getY() { return y; }

    void applyMove();

    bool isAlive() {
        return lifetime != 0;
    }

    bool isBullet() {
        return lifetime >=0;
    }

    void setLifetime (int lt) {
        lifetime = lt;
    }

    void update() {

        if( lifetime > 0) {
            lifetime--;
        }
        applyMove();

    }


    void setName(const char *txt) {
        ACE_OS::strncpy(name,txt,sizeof(name));
        for (unsigned int i=0; i<ACE_OS::strlen(name); i++) {
            char ch = name[i];
            if (!((ch>='a'&&ch<='z')||
                  (ch>='A'&&ch<='Z')||
                  (ch>='0'&&ch<='9')||
                  ch=='/')) {
                name[i] = '_';
            }
        }
    }
    const char *getName() {
        return name;
    }

    void setLife(int l) { life = l; if (life<0) life=0; };
    int  getLife() {return life; };
};

#endif
