// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 IITRBCS
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <stdio.h>

#include <Demo.h>
#include <Tennis.h>
#include <Rpc.h>
#include <SharedData.h>
#include <yarp/os/all.h>

using namespace yarp::os;

bool test_signs() {
    printf("\n*** test_signs()\n");
    Demo demo;
    demo.an_unsigned_byte = 254;
    if (demo.an_unsigned_byte<0) {
        printf("Oops, an_unsigned_byte is signed\n");
        return false;
    }
    demo.a_signed_byte = -1;
    if (demo.a_signed_byte>0) {
        printf("Oops, a_signed_byte is unsigned\n");
        return false;
    }
    printf("\n*** ok!\n");
    return true;
}

bool test_serialization() {
   printf("\n*** test_serialization()\n");
   SharedData data;
   data.text = "hello";
   data.content.push_back(1);
   data.content.push_back(2);
   Bottle bot;
   bot.read(data);
   if (bot.get(0).asString()!="hello") {
       printf("Oops, string is not right\n");
       return false;
   }
   if (bot.get(1).asList()==NULL) {
       printf("Oops, list is not right\n");
       return false;
   }
   if (bot.get(1).asList()->size()!=2) {
       printf("Oops, length is not right\n");
       return false;
   }
   printf("*** %s (ok!)\n", bot.toString().c_str());
   return true;
}


int main(int argc, char *argv[]) {
    if (!test_signs()) return 1;
    if (!test_serialization()) return 1;

    return 0;
}
