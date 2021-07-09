/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Bottle.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>

#include <cstdio>

using yarp::os::Bottle;
using yarp::os::Value;

void showBottle(Bottle& anUnknownBottle, int indentation = 0)
{
    for (size_t i = 0; i < anUnknownBottle.size(); i++) {
        for (int j = 0; j < indentation; j++) {
            printf(" ");
        }
        printf("[%zu]: ", i);
        Value& element = anUnknownBottle.get(i);
        switch (element.getCode()) {
        case BOTTLE_TAG_INT32:
            printf("int %d\n", element.asInt32());
            break;
        case BOTTLE_TAG_FLOAT64:
            printf("float %g\n", element.asFloat64());
            break;
        case BOTTLE_TAG_STRING:
            printf("string \"%s\"\n", element.asString().c_str());
            break;
        case BOTTLE_TAG_BLOB:
            printf("binary blob of length %zd\n", element.asBlobLength());
            break;
        case BOTTLE_TAG_VOCAB32:
            printf("vocab [%s]\n", yarp::os::Vocab32::decode(element.asVocab32()).c_str());
            break;
        default:
            if (element.isList()) {
                Bottle* lst = element.asList();
                printf("list of %zu elements\n", lst->size());
                showBottle(*lst, indentation + 2);
            } else {
                printf("unrecognized type\n");
            }
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    Bottle anUnknownBottle("equals 7 (add (add 2 3) 2)");
    showBottle(anUnknownBottle);
    return 0;
}
