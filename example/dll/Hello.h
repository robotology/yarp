/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
