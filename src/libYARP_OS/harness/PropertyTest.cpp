// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#include <yarp/os/Property.h>

#include "TestList.h"

using namespace yarp;
using namespace yarp::os;

class PropertyTest : public UnitTest {
public:
    virtual String getName() { return "PropertyTest"; }

    void checkPutGet() {
        report(0,"checking puts and gets");
        Property p;
        p.put("hello","there");
        p.put("hello","friend");
        p.put("x","y");
        checkTrue(p.check("hello"), "key 1 exists");
        checkTrue(p.check("x"), "key 2 exists");
        checkTrue(!(p.check("y")), "other key should not exist");
        checkEqual(p.find("hello").toString().c_str(),"friend", 
                   "key 1 has good value");
        checkEqual(p.find("x").toString().c_str(),"y", 
                   "key 2 has good value");
        p.fromString("(hello)");
        checkTrue(p.check("hello"), "key exists");
        Value *v;
        checkFalse(p.check("hello",v), "has no value");
    }


    void checkTypes() {
        report(0,"checking puts and gets of various types");
        Property p;
        p.put("ten",10);
        p.put("pi",(double)3.14);
        checkEqual(p.find("ten").asInt(),10,"ten");
        checkTrue(p.find("pi").asDouble()>3,"pi>3");
        checkTrue(p.find("pi").asDouble()<4,"pi<4");
        p.unput("ten");
        checkTrue(p.find("ten").isNull(),"unput");
    }

    void checkExternal() {
        report(0,"checking external forms");
        Property p;
        p.fromString("(foo 12) (testing left right)");
        checkEqual(p.find("foo").asInt(),12,"good key 1");
        checkEqual(p.find("testing").asString().c_str(),"left","good key 2");
        checkEqual(p.findGroup("testing").toString().c_str(),
                   "testing left right","good key 2 (more)");

        Property p2;
        p2.fromString(p.toString().c_str());
        checkEqual(p.find("testing").asString().c_str(),"left","good key after copy");

        Property p3;
        char *args[] = {"CMD","--size","10","20","--mono","on"};
        p3.fromCommand(5,args);
        Bottle bot(p3.toString().c_str());
        checkEqual(bot.size(),2,"right number of terms");
        checkEqual(p3.findGroup("size").get(1).toString().c_str(),"10","width");
        checkEqual(p3.findGroup("size").get(2).toString().c_str(),"20","height");
        checkTrue(p3.findGroup("size").get(1).isInt(),"width type");
        checkEqual(p3.findGroup("size").get(1).asInt(),10,"width type val");

        report(0,"reading from config-style string");
        Property p4;
        p4.fromConfig("size 10 20\nmono on\n");
        Bottle bot2(p4.toString().c_str());
        checkEqual(bot2.size(),2,"right number of terms");
        checkEqual(p4.findGroup("size").get(1).toString().c_str(),"10","width");
        checkEqual(p4.findGroup("size").get(2).toString().c_str(),"20","height");
        checkTrue(p4.findGroup("size").get(1).isInt(),"width type");
        checkEqual(p4.findGroup("size").get(1).asInt(),10,"width type val");

        report(0,"more realistic config-style string");
        Property p5;
        p5.fromConfig("[cat1]\nsize 10 20\nmono on\n[cat2]\nfoo\t100\n");
        Bottle bot3(p5.toString().c_str());
        checkEqual(bot3.size(),2,"right number of terms");
        checkEqual(p5.findGroup("cat1").findGroup("size").get(1).asInt(),
                   10,"category 1, size, width");
        checkEqual(p5.findGroup("cat2").findGroup("foo").get(1).asInt(),
                   100,"category 2, foo");

        report(0,"command line style string");
        Property p6;
        char *strs[] = { "program", "--name", "/foo" };
        p6.fromCommand(3,strs);
        checkEqual(p6.find("name").asString().c_str(),"/foo",
                   "command line name");
        Value *v = NULL;
        p6.check("name",v);
        checkTrue(v!=NULL,"check method");


        Searchable *network = &p6.findGroup("NETWORK");
        if (network->isNull()) { network = &p6; }
        v = NULL;
        network->check("name",v);
        checkTrue(v!=NULL,"check method 2");
    }

    virtual void runTests() {
        checkPutGet();
        checkExternal();
        checkTypes();
    }
};

static PropertyTest thePropertyTest;

UnitTest& getPropertyTest() {
    return thePropertyTest;
}
