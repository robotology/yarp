// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
    long int asInt() const {
        return x;
    }
    bool isValid() {
        return x!=-1;
    }
};

#endif
