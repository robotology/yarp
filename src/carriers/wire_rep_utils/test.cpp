// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdio.h>

#include "WireTwiddler.h"

#include <yarp/os/impl/StringInputStream.h>
#include <yarp/os/impl/Route.h>
#include <yarp/os/InputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

using namespace yarp::os;
using namespace yarp::os::impl;

bool testSequence(char *seq, int len, const char *fmt, Bottle ref,
                  bool testWrite = true) {
    printf("\n");
    printf("================================================\n");
    printf(" READ %s\n", fmt);
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
    printf("[1] %s: read %s as expected\n", fmt, bot.toString().c_str());


    StringInputStream sis;
    sis.add(b1);
    sis.add(b1);
    WireTwiddlerReader twiddled_input(sis,tt);
    Route route;
    StreamConnectionReader reader2;
    reader2.reset(twiddled_input,NULL,route,0,false);
    bot.clear();
    twiddled_input.reset();
    bot.read(reader2);

    if (bot!=ref) {
        printf("%s: read %s, expected %s\n", fmt, 
               bot.toString().c_str(),
               ref.toString().c_str());
        printf("MISMATCH\n");
        exit(1);
        return false;
    }
    printf("[2] %s: read %s as expected\n", fmt, bot.toString().c_str());


    StreamConnectionReader reader3;
    reader3.reset(twiddled_input,NULL,route,0,false);
    bot.clear();
    twiddled_input.reset();
    bot.read(reader3);

    if (bot!=ref) {
        printf("%s: read %s, expected %s\n", fmt, 
               bot.toString().c_str(),
               ref.toString().c_str());
        printf("MISMATCH\n");
        exit(1);
        return false;
    }
    printf("[3] %s: read %s as expected\n", fmt, bot.toString().c_str());

    if (testWrite) {
        
        printf("\n");
        printf("================================================\n");
        printf(" WRITE %s\n", fmt);
        ManagedBytes output;
        tt.write(ref,output);
        if (output.length()!=(size_t)len) {
            fprintf(stderr,"WRITE MISMATCH, length %d, expected %d\n",
                    (int)output.length(), len);
            exit(1);
            return false;
        }
        for (size_t i=0; i<output.length(); i++) {
            if (output.get()[i] != seq[i]) {
                fprintf(stderr,"WRITE MISMATCH, at %d, have [%d:%c] expected [%d:%c]\n",
                        (int)i, output.get()[i], output.get()[i], seq[i], seq[i]);
                exit(1);
                return false;
            }
        }
        printf("[4] %s: wrote %s as expected\n", fmt, bot.toString().c_str());
    }

    return true;
}

int main(int argc, char *argv[]) {

    if (argc==1) {
        {
            char seq[] = {42,0,0,0};
            testSequence(seq,sizeof(seq),"vector int32 1 *",Bottle("42"));
        }
        {
            char seq[] = {6,0,0,0,'h','e','l','l','o','\0'};
            testSequence(seq,sizeof(seq),"vector string 1 *",Bottle("hello"));
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
            char seq[] = {6,0,0,0,'h','e','l','l','o','\0',12,0,0,0};
            testSequence(seq,sizeof(seq),"list 2 string * int32 *",Bottle("hello 12"));
        }
        {
            char seq[] = {6,0,0,0,'h','e','l','l','o','\0',12,0,0,0,42,0,0,0};
            testSequence(seq,sizeof(seq),"list 3 string * int32 * int32 *",Bottle("hello 12 42"));
        }
        {
            char seq[] = {6,0,0,0,'h','e','l','l','o','\0',12,0,0,0,42,0,0,0};
            testSequence(seq,sizeof(seq),"list 2 string * skip int32 * int32 *",Bottle("hello 42"),false);
        }
        {
            char seq[] = {33,0,0,0,42,0,0,0};
            testSequence(seq,sizeof(seq),"list 1 skip int32 * int32 *",Bottle("42"),false);
        }
        {
            char seq[] = {99,0,0,0,42,0,0,0};
            testSequence(seq,sizeof(seq),"skip int32 * list 1 int32 *",Bottle("42"),false);
        }
    }
    if (argc==2) {
        WireTwiddler tt;
        Bottle b(argv[1]);
        printf(">>> %s\n", tt.fromTemplate(b).c_str());
    }

    return 0;
}

