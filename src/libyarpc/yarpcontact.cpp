/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

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
    YARP_CONTACT(contact) = Contact(name);
    return 0;
}
