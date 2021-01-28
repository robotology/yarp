/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef ID_H
#define ID_H

class ID {
public:
    long int x;
    ID() { x = -1; }
    ID(int nx) { x = nx; }
    bool operator == (const ID& alt) const {
        return x == alt.x;
    }
    long int hash() const {
        return x;
    }
    long int asInt32() const {
        return x;
    }
    bool isValid() {
        return x!=-1;
    }
};

#endif
