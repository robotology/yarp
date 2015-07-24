// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 IITRBCS
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/serversql/yarpserversql.h>

int main(int argc, char *argv[]) {
    int ret=yarpserver3_main(argc, argv);
    return (ret!=0?1:0);
}

