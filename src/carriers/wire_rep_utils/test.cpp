// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>

#include "WireTwiddler.h"

using namespace yarp::os;

int main(int argc, char *argv[]) {
    if (argc!=2) {
        fprintf(stderr,"Specify a bottle\n");
        return 1;
    }
    WireTwiddler tt;
    tt.configure(argv[1]);
    return 0;
}

