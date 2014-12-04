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
#include <HeaderTest.h>
#include <HeaderTest2.h>
#include <yarp/os/all.h>
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class RosMsgTest : public UnitTest {
public:
    virtual ConstString getName() {
        return "RosMsg";
    }
};

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

template <class T>
bool test_lists(UnitTest& test, T demo, std::string name) {
    test.report(0,std::string("test lists ") + name);
    Port p1;
    p1.enableBackgroundWrite(true);
    if (!p1.open("/p1")) return false;
    BufferedPort<T> p2;
    if (!p2.open("/p2")) return false;
    if (!Network::connect(p1.getName(),p2.getName())) {
        printf("Could not connect\n");
        return false;
    }
    demo.x = 10;
    demo.a_signed_byte = 0;
    demo.an_unsigned_byte = 0;
    demo.a_signed_byte2 = 0;
    demo.an_unsigned_byte2 = 0;
    demo.a_signed_byte4 = 0;
    demo.an_unsigned_byte4 = 0;
    demo.a_signed_byte8 = 0;
    demo.an_unsigned_byte8 = 0;
    demo.a_bool = false;
    for (int i=0; i<3; i++) {
        demo.fixed_string_list[i] = "";
        demo.fixed_byte_list[i] = 1;
    }
    demo.fixed_string_list[2] = "ping";
    demo.fixed_byte_list[1] = 99;
    p1.write(demo);
    T *in = p2.read();
    test.checkTrue(in!=NULL,"read ok");
    if (!in) return false;
    test.checkEqual(demo.x,in->x,"int ok");
    test.checkEqual(demo.fixed_string_list[2].c_str(),in->fixed_string_list[2].c_str(),"string in list ok");
    test.checkEqual(demo.fixed_byte_list[1],in->fixed_byte_list[1],"byte in list ok");
    p2.close();
    p1.close();
    return test.isOk();
}

int main(int argc, char *argv[]) {
    Network yarp;
    yarp.setLocalMode(true);
    UnitTest::startTestSystem();
    RosMsgTest test;

    if (!test_signs()) return 1;
    if (!test_serialization()) return 1;
    
    Demo demo1;
    if (!test_lists(test,demo1,"regular")) return 1;
    Demo::bottleStyle demo2;
    if (!test_lists(test,demo2,"bottle")) return 1;
    Demo::rosStyle demo3;
    if (!test_lists(test,demo3,"ros")) return 1;
    UnitTest::stopTestSystem();

    return 0;
}
