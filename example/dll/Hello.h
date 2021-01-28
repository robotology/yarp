/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef HELLO_INC
#define HELLO_INC

class Hello {
public:
    virtual void write(int v) = 0;
    virtual int read() = 0;
};

class HelloImpl : public Hello {
private:
    int x;
public:
    HelloImpl() { x = 0; }
    void write(int v);
    int read();
};

#endif
