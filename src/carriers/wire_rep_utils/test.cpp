// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdio.h>

#include "WireTwiddler.h"

using namespace yarp::os;

bool testSequence(char *seq, int len, const char *fmt, Bottle ref) {
    Bytes b1(seq,len);
    WireTwiddler tt;
    tt.configure(fmt);
    printf(">>> %s\n", tt.toString().c_str());
    Bottle bot;
    if (!tt.read(bot,b1)) {
        fprintf(stderr,"Read failed\n");
        return 1;
    }
    if (bot!=ref) {
        printf("%s: read %s, expected %s\n", fmt, 
               bot.toString().c_str(),
               ref.toString().c_str());
        printf("MISMATCH\n");
        exit(1);
        return false;
    }
    printf("%s: read %s as expected\n", fmt, bot.toString().c_str());
    return true;
}

int main(int argc, char *argv[]) {
    if (argc!=2) {
        fprintf(stderr,"Specify a bottle\n");
        return 1;
    }
    WireTwiddler tt;
    tt.configure(argv[1]);
    printf(">>> %s\n", tt.toString().c_str());

    {
        char seq[] = {42,0,0,0};
        testSequence(seq,sizeof(seq),"list 1 int32 *",Bottle("42"));
    }
    {
        char seq[] = {42,0,0,0,55,0,0,0};
        testSequence(seq,sizeof(seq),"list 2 int32 * int32 *",Bottle("42 55"));
    }
    {
        char seq[] = {2,0,0,0,42,0,0,0,55,0,0,0};
        testSequence(seq,sizeof(seq),"vector int32 *",Bottle("42 55"));
    }
    {
        char seq[] = {2,0,0,0,4,0,0,0,'f','o','o','\0',4,0,0,0,'b','a','r','\0'};
        testSequence(seq,sizeof(seq),"vector string *",Bottle("foo bar"));
    }
    {
        char seq[] = {2,0,0,0,4,0,0,0,'f','o','o','\0',4,0,0,0,'b','a','r','\0',12,0,0,0};
        testSequence(seq,sizeof(seq),"list 2 vector string * int32 *",Bottle("(foo bar) 12"));
    }
    {
        char seq[] = {2,0,0,0,4,0,0,0,'f','o','o','\0',4,0,0,0,'b','a','r','\0',12,0,0,0,2,0,0,0,42,0,0,0,24,0,0,0};
        testSequence(seq,sizeof(seq),"list 3 vector string * int32 * vector int32 *",Bottle("(foo bar) 12 (42 24)"));
    }
    {
        char seq[] = {42,0,0,0,24,0,0,0};
        testSequence(seq,sizeof(seq),"vector int32 2 *",Bottle("42 24"));
    }

    {
        printf("Try a write...\n");
        ManagedBytes seq;
        tt.configure("vector int32 2 *");
        Bottle b("42,24");
        tt.write(b,seq);
    }

    return 0;
}

