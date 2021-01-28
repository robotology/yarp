/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef HANDLER_INC
#define HANDLER_INC

class Handler {
public:
    virtual ~Handler() {
    }

    virtual int hello() {
        return 99;
    }
};

#endif
