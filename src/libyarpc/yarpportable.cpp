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
     * Initialize a portable structure.
     *
     */
YARP_DEFINE(int) yarpPortableInit(yarpPortablePtr portable) {
    if (portable!=NULL) {
        portable->adaptor = NULL;
        portable->client = NULL;
        portable->callbacks = NULL;
    }
    return 0;
}


    /**
     *
     * Finalize a portable structure.
     *
     */
YARP_DEFINE(int) yarpPortableFini(yarpPortablePtr portable) {
    if (portable!=NULL) {
        if (portable->adaptor!=NULL) {
            delete (YarpImplPortableAdaptor*)(portable->adaptor);
            portable->adaptor = NULL;
        }
        portable->client = NULL;
        portable->callbacks = NULL;
    }
    return 0;
}

    /**
     *
     * Initialize a portable callbacks structure.
     *
     */
YARP_DEFINE(int) yarpPortableCallbacksInit(yarpPortableCallbacksPtr callbacks) {
    if (callbacks!=NULL) {
        callbacks->read = NULL;
        callbacks->write = NULL;
        callbacks->onCompletion = NULL;
        callbacks->onCommencement = NULL;
    }
    return 0;
}

    /**
     *
     * set the write handler of a portable structure.
     *
     */
/*
YARP_DEFINE(int) yarpPortableSetWriteHandler(yarpPortablePtr portable, int (*write) (yarpWriterPtr connection, void *impl)) {
    YARP_OK(portable);
    YARP_OK(portable->callbacks);
    portable->callbacks->write = write;
    return 0;
}
*/

    /**
     *
     * set the read handler of a portable structure.
     *
     */
/*
YARP_DEFINE(int) yarpPortableSetReadHandler(yarpPortablePtr portable, int (*read) (yarpReaderPtr connection, void *impl)) {
    YARP_OK(portable);
    YARP_OK(portable->callbacks);
    portable->callbacks->read = read;
    return 0;
}
*/


    /**
     *
     * set the onCompletion handler of a portable structure.
     *
     */
/*
YARP_DEFINE(int) yarpPortableSetOnCompletionHandler(yarpPortablePtr portable, int(*onCompletion)(void *impl)) {
    YARP_OK(portable);
    YARP_OK(portable->callbacks);
    portable->callback->onCompletion = onCompletion;
    return 0;
}
*/

    /**
     *
     * set the onCommencement handler of a portable structure.
     *
     */
/*
YARP_DEFINE(int) yarpPortableSetOnCommencementHandler(yarpPortablePtr portable, int(*onCommencement)(void *impl)) {
    YARP_OK(portable);
    YARP_OK(portable->callbacks);
    portable->callbacks->onCommencement = onCommencement;
    return 0;
}
*/


