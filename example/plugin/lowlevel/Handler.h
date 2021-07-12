/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
