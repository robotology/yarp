/*
 * Copyright: (C) 2012 iCub Facility
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef HANDLER_INC
#define HANDLER_INC

class Handler {
public:
    virtual ~Handler() {
    }
    
    virtual int hello() {
        return 99;
    }
};

#endif
