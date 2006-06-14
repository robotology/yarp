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
        checkEqual(p.get("hello").toString().c_str(),"friend", 
                   "key 1 has good value");
        checkEqual(p.get("x").toString().c_str(),"y", 
                   "key 2 has good value");
    }

    void checkExternal() {
        report(0,"checking external forms");
        Property p;
        p.fromString("(foo 12) (testing left right)");
        checkEqual(p.getString("foo").c_str(),"12","good key 1");
        checkEqual(p.getString("testing").c_str(),"left","good key 2");
        checkEqual(p.getList("testing")->toString().c_str(),
                   "testing left right","good key 2 (more)");

        Property p2;
        p2.fromString(p.toString().c_str());
        checkEqual(p.getString("testing").c_str(),"left","good key after copy");

        Property p3;
        char *args[] = {"CMD","--size","10","20","--mono","on"};
        p3.fromCommand(5,args);
        Bottle bot(p3.toString().c_str());
        checkEqual(bot.size(),2,"right number of terms");
        checkEqual(p3.getList("size")->get(1).toString().c_str(),"10","width");
        checkEqual(p3.getList("size")->get(2).toString().c_str(),"20","height");
        checkTrue(p3.getList("size")->get(1).isInt(),"width type");
        checkEqual(p3.getList("size")->get(1).asInt(),10,"width type val");

        report(0,"reading from config-style string");
        Property p4;
        p4.fromConfig("size 10 20\nmono on\n");
        Bottle bot2(p4.toString().c_str());
        checkEqual(bot2.size(),2,"right number of terms");
        checkEqual(p4.getList("size")->get(1).toString().c_str(),"10","width");
        checkEqual(p4.getList("size")->get(2).toString().c_str(),"20","height");
        checkTrue(p4.getList("size")->get(1).isInt(),"width type");
        checkEqual(p4.getList("size")->get(1).asInt(),10,"width type val");

        report(0,"more realistic config-style string");
        Property p5;
        p5.fromConfig("[cat1]\nsize 10 20\nmono on\n[cat2]\nfoo bar\n");
        Bottle bot3(p5.toString().c_str());
        checkEqual(bot3.size(),2,"right number of terms");
        checkTrue(p5.getList("cat1")!=NULL,"category 1");
        checkEqual(p5.getList("cat1")->findGroup("size").get(1).asInt(),
                   10,"category 1, size, width");
    }

    virtual void runTests() {
        checkPutGet();
        checkExternal();
    }
};

static PropertyTest thePropertyTest;

UnitTest& getPropertyTest() {
    return thePropertyTest;
}
