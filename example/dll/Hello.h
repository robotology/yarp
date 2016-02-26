/*
 * Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
