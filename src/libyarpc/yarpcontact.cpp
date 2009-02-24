// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <stdio.h>

#include "yarp.h"
#include "yarpimpl.h"

    /**
     *
     * Create an empty contact.
     *
     */
YARP_DEFINE(yarpContactPtr) yarpContactCreate() {
    yarpContactPtr contact = new yarpContact;
    if (contact!=NULL) {
        contact->implementation = new Contact();
        if (contact->implementation==NULL) {
            delete contact;
            contact = NULL;
        }
    }
    return contact;
}


    /**
     *
     * Destroy a contact.
     *
     */
YARP_DEFINE(void) yarpContactFree(yarpContactPtr contact) {
    if (contact!=NULL) {
        if (contact->implementation!=NULL) {
            delete (Contact*)(contact->implementation);
            contact->implementation = NULL;
        }
        delete contact;
    }
}

    /**
     *
     * Set the port name of a contact.
     *
     */
YARP_DEFINE(int) yarpContactSetName(yarpContactPtr contact,
                                    const char *name) {
    YARP_OK(contact);
    // contact class is very awkward
    YARP_CONTACT(contact) = Contact::byName(name);
    return 0;
}





