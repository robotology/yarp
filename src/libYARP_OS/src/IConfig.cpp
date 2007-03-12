// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/IConfig.h>
#include <yarp/os/all.h>

using namespace yarp::os;

bool IConfig::openFromCommand(int argc, char *argv[], bool skipFirst) {
    Property options;
    options.fromCommand(argc,argv,skipFirst);

    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file",val)) {
        ConstString fname = val->toString();
        options.unput("file");
        printf("Working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname,false);

        // interpret command line options as a set of flags again
        // (just in case we need to override something)
        options.fromCommand(argc,argv,true,false);
    }

    // check if we want to use nested options (less ambiguous)
    if (options.check("nested",val)||options.check("lispy",val)) {
        ConstString lispy = val->toString();
        options.fromString(lispy);
    }
    
    return open(options);
}




