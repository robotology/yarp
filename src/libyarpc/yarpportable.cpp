/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>
#include <cstring>

#include "yarp.h"
#include "yarpimpl.h"

static yarpPortableCallbacks __portable_callbacks;


#define SET_CALLBACK(name,target,source,fallback) if (source->name) { target.name = source->name; } else { target.name = fallback; }

static int __default_write(yarpWriterPtr connection, void *client) {
    return -1;
}

static int __default_read(yarpReaderPtr connection, void *client) {
    return -1;
}

static int __default_onCommencement(void *client) {
    return -1;
}

static int __default_onCompletion(void *client) {
    return -1;
}

static int __installed_callbacks = 0;

YARP_DEFINE(int) yarpPortableCallbacksInstall(yarpPortableCallbacksPtr
                                              callbacks) {
    if (!__installed_callbacks) {
        yarpPortableCallbacksComplete(callbacks);
        memcpy(&__portable_callbacks,callbacks,sizeof(yarpPortableCallbacks));
        __installed_callbacks = 1;
    }
    return 0;
}

YARP_DEFINE(int) yarpPortableCallbacksComplete(yarpPortableCallbacksPtr
                                               callbacks) {
    if (!callbacks->write) callbacks->write = __default_write;
    if (!callbacks->read) callbacks->read = __default_read;
    if (!callbacks->onCompletion) callbacks->onCompletion = __default_onCompletion;
    if (!callbacks->onCommencement) callbacks->onCommencement = __default_onCommencement;
    return 0;
}

YARP_DEFINE(yarpPortableCallbacksPtr) yarpPortableCallbacksGet() {
    return &__portable_callbacks;
}

    /**
     *
     * Initialize a portable structure.
     *
     */
YARP_DEFINE(int) yarpPortableInit(yarpPortablePtr portable,
                                  yarpPortableCallbacksPtr callbacks) {
    if (portable!=NULL) {
        portable->implementation = NULL;
        portable->adaptor= NULL;
    }
    if (callbacks!=NULL && !__installed_callbacks) {
        portable->adaptor = new YarpImplPortableAdaptor(portable,
                                                        callbacks);
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
        if (portable->implementation!=NULL) {
            delete (Portable*)(portable->implementation);
            portable->implementation = NULL;
        }
        portable->client = NULL;
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
        callbacks->unused1 = NULL;
        callbacks->unused2 = NULL;
        callbacks->unused3 = NULL;
        callbacks->unused4 = NULL;
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
