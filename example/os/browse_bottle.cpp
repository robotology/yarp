/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
using namespace yarp::os;

void showBottle(Bottle& anUnknownBottle, int indentation = 0) {
    for (int i=0; i<anUnknownBottle.size(); i++) {
        for (int j=0; j<indentation; j++) { printf(" "); }
        printf("[%d]: ", i);
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
        case BOTTLE_TAG_VOCAB:
            printf("vocab [%s]\n", Vocab::decode(element.asVocab()).c_str());
            break;
        default:
            if (element.isList()) {
                Bottle *lst = element.asList();
                printf("list of %zu elements\n", lst->size());
                showBottle(*lst,indentation+2);
            } else {
                printf("unrecognized type\n");
            }
            break;
        }
    }
}

int main() {
    Bottle anUnknownBottle("equals 7 (add (add 2 3) 2)");
    showBottle(anUnknownBottle);
    return 0;
}
