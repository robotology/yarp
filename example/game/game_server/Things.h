/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef THINGS_H
#define THINGS_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>

#include "Matrix.h"
#include "Thing.h"


typedef ACE_Hash_Map_Manager<ID,Thing,ACE_Null_Mutex> hthing;
typedef ACE_Hash_Map_Entry<ID,Thing> hthing_entry;

class Things {
public:
    hthing things;
    hid created;
    hid destroyed;
    ID at;
    static const ID theta;
    Thing null_thing;

    Things() {
        at = theta;
    }

    static bool isFluid(ID x) {
        return x.asInt32()>=theta.asInt32();
    }

    ID create() {
        // could also reuse
        ID result = at;
        created.bind(at,at);
        things.bind(at,Thing());
        at = at.asInt32()+1;
        return result;
    }

    ID create(ID nat) {
        // could also reuse
        created.bind(nat,nat);
        things.bind(nat,Thing());
        getThing(nat).setID(nat);
        return nat;
    }

    Thing& getThing(ID x) {
        hthing_entry *entry;
        if (0==things.find(x,entry)) {
            return entry->int_id_;
        }
        return null_thing;
    }

    bool isThing(ID x) {
        hthing_entry *entry;
        if (0==things.find(x,entry)) {
            return true;
        }
        return false;
    }

    void destroy(ID x) {
        // prepare for reuse
        things.unbind(x);
        created.unbind(x,x);
        destroyed.bind(x,x);
    }

    bool isDestroyed(ID x) {
        ID result;
        return destroyed.find(x,result)==0;
    }

    void update() {
        hid remove;
        for (hid_iterator it=created.begin(); it!=created.end(); it++) {
            ID id = (*it).ext_id_;
            Thing& thing = getThing(id);

            thing.update();

            if(!thing.isAlive()) {
                // remove.bind(id,id); // for now, don't do this
                // Players automatically expire from arena when client killed
            }
        }
        for (hid_iterator it2=remove.begin(); it2!=remove.end(); it2++) {
            ID id = (*it2).ext_id_;
            destroy(id);
        }

    }
};



#endif
