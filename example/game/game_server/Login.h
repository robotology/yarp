/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef LOGIN_INC
#define LOGIN_INC

#include <stdio.h>
#include "Thing.h"

class Login {
public:
    Login();
    bool apply(const char *name, const char *key);
    void shutdown();
    ID getID() {
        return base_id;
    }
    Thing& getThing();

private:
    ID base_id;
};

#endif
