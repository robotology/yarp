// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Hello.h"
#include "ShareHello.h"

#include <yarp/os/SharedLibrary.h>

#include <stdio.h>

using namespace yarp::os;

int main(int argc, char *argv[]) {
    SharedLibrary lib;
    lib.open("hello");
    if (!lib.isValid()) {
        fprintf(stderr,"Failed to open library\n");
        fprintf(stderr,"Make sure libhello.so/hello.dll/... is in your library path\n");
        return 1;
    }
    void *fn = lib.getSymbol("hello_there");
    if (fn==NULL) {
        fprintf(stderr,"Failed to find factory in library\n");
        return 1;
    }
    SharedLibraryClassFactory<Hello> helloFactory(fn);
    SharedLibraryClass<Hello> *hello = helloFactory.wrap();
    if (hello==NULL) {
        fprintf(stderr,"Failed to create Hello object\n");
        return 1;
    }
    hello->getContent().write(42);
    printf("42 + 1 = %d\n", hello->getContent().read());
    delete hello;
    return 0;
}


