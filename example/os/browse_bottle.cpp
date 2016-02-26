/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
        case BOTTLE_TAG_INT:
            printf("int %d\n", element.asInt());
            break;
        case BOTTLE_TAG_DOUBLE:
            printf("float %g\n", element.asDouble());
            break;
        case BOTTLE_TAG_STRING:
            printf("string \"%s\"\n", element.asString().c_str());
            break;
        case BOTTLE_TAG_BLOB:
            printf("binary blob of length %d\n", element.asBlobLength());
            break;
        case BOTTLE_TAG_VOCAB:
            printf("vocab [%s]\n", Vocab::decode(element.asVocab()).c_str());
            break;
        default:
            if (element.isList()) {
                Bottle *lst = element.asList();
                printf("list of %d elements\n", lst->size());
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
