/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>

using namespace yarp::os;

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Bottle b;
    b.addString("color");
    b.addString("red");
    printf("Bottle b is: %s\n", b.toString().c_str());
    // should give: color red

    Bottle b2;
    b2.addString("height");
    b2.addInt32(15);
    printf("Bottle b2 is: %s\n", b2.toString().c_str());
    // should give: height 15

    Bottle b3;
    b3.addList() = b;
    b3.addList() = b2;
    printf("Bottle b3 is: %s\n", b3.toString().c_str());
    // should give: (color red) (height 15)

    printf("color check: %s\n", b3.find("color").asString().c_str());
    printf("height check: %d\n", b3.find("height").asInt32());

    Bottle b4;
    b4.addString("nested");
    b4.addList() = b3;
    printf("Bottle b4 is: %s\n", b4.toString().c_str());
    // should give: nested ((color red) (height 15))

    // alternative way to create a Bottle from textual representation
    Bottle b5("(pos left top) (size 10)");
    printf("Bottle b5 is: %s\n", b5.toString().c_str());
    // should give: (pos left top) (size 10)

    Bottle b6;
    b6 = b5;
    b6.addList() = b4;
    printf("Bottle b6 is: %s\n", b6.toString().c_str());
    // should give: (pos left top) (size 10) (nested ((color red) (height 5))

    printf("size check: %d\n", b6.find("size").asInt32());
    printf("pos check: %s\n", b6.find("pos").asString().c_str());
    // find assumes key->value pairs; for lists, use findGroup
    printf("pos group check: %s\n", b6.findGroup("pos").toString().c_str());
    // see documentation for Bottle::findGroup
    printf("nested check: %s\n", b6.find("nested").toString().c_str());
    printf("nested height check: %d\n", b6.find("nested").find("height").asInt32());


    printf("\n");
    printf("Relationship of Bottle and Property\n");
    Property subProp;
    subProp.put("hello","there");
    subProp.put("fortytwo",42);
    printf("subProp: %s\n", subProp.toString().c_str());

    Value *lst = Value::makeList();
    if (lst == nullptr) {
        printf("Failed to allocate list\n");
        return 1;
    }
    lst->asList()->fromString(subProp.toString());
    printf("lst: %s\n", lst->toString().c_str());

    Property prop;
    prop.put("height",15);
    prop.put("verbose",1);
    prop.put("sub",lst);
    printf("prop: %s\n", prop.toString().c_str());

    return 0;
}
