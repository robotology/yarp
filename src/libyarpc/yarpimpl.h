// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YARPC_YARPIMPL_INC
#define YARPC_YARPIMPL_INC

#include "yarp.h"

#include <yarp/os/all.h>
using namespace yarp::os;

#define YARP_DEFINE(rt) rt

#define YARP_OK(s) if(s->implementation==NULL) return -1;
#define YARP_PORT(s) (*((Port*)(s->implementation)))
#define YARP_CONTACT(s) (*((Contact*)(s->implementation)))
#define YARP_NETWORK(s) (*((Network*)(s->implementation)))
#define YARP_READER(s) (*((ConnectionReader*)(s->implementation)))
#define YARP_WRITER(s) (*((ConnectionWriter*)(s->implementation)))

#endif
