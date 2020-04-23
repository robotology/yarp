/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/SplitString.h>

#include <cstdlib>
#include <cstring>


using yarp::os::impl::SplitString;

SplitString::SplitString() :
        argc(0)
{
    for (auto& i : argv) {
        i = nullptr;
    }
}

SplitString::SplitString(const char* command, const char splitter)
{
    apply(command, splitter);
}

int SplitString::size()
{
    return argc;
}

void SplitString::set(int index, const char* txt)
{
    if (index >= 0 && index < size()) {
        strncpy(buf[index], (char*)txt, MAX_ARG_LEN - 1);
        buf[index][MAX_ARG_LEN - 1] = '\0';
    }
}

const char* SplitString::get(int idx)
{
    return buf[idx];
}

const char** SplitString::get()
{
    return (const char**)argv;
}

void SplitString::apply(const char* command, char splitter)
{
    size_t at = 0;
    size_t sub_at = 0;
    unsigned int i;
    for (i = 0; i < strlen(command) + 1; i++) {
        if (at < MAX_ARG_CT) {
            // yarpserver can deal with quoting.
            char ch = command[i];
            if (ch == '\"') {
                ch = ' ';
            }
            if (ch >= 32 || ch == '\0' || ch == '\n') {
                if (ch == splitter || ch == '\n') {
                    ch = '\0';
                }
                if (sub_at < MAX_ARG_LEN) {
                    buf[at][sub_at] = ch;
                    sub_at++;
                }
            }
            if (ch == '\0') {
                if (sub_at > 1) {
                    at++;
                }
                sub_at = 0;
            }
        }
    }
    for (i = 0; i < MAX_ARG_CT; i++) {
        argv[i] = buf[i];
        buf[i][MAX_ARG_LEN - 1] = '\0';
    }

    argc = at;
}
