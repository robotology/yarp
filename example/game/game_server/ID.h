/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
