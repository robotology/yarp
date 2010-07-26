// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_WIREBOTTLE
#define YARP2_WIREBOTTLE

class WireBottle {
public:
    static bool checkBottle(void *cursor, int len);
};

#endif
