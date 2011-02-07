// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHAREDLIBRARYCLASS_
#define _YARP2_SHAREDLIBRARYCLASS_

namespace yarp {
    namespace os {
        template <class T>
        class SharedLibraryClass;

        template <class T>
        class SharedLibraryClassFactory;

        struct SharedLibraryClassApi;

        extern "C" {
            typedef void (*SharedClassFactoryFunction)(void *ptr);
            typedef void (*SharedClassDeleterFunction)(void *ptr);
        }
  }
}

#include <yarp/os/Vocab.h>

// Be careful loading C++ classes from DLLs.  Generally you
// need an exact or very close match between compilers used
// to compile those DLLs and your own code.

#include <yarp/os/begin_pack_for_net.h>
extern "C" {
    struct yarp::os::SharedLibraryClassApi {
    public:
        NetInt32 startCheck; // should be 'Y' 'A' 'R' 'P'
        void *(*create)();
        void (*destroy)(void *obj);
        int (*getVersion)(char *ver, int len);
        int (*getAbi)(char *abi, int len);
        NetInt32 roomToGrow[32];
        NetInt32 endCheck;   // should be 'P' 'R' 'A' 'Y'
    };
}
#include <yarp/os/end_pack_for_net.h>

#define YARP_DECLARE_SHARED_CLASS(factoryname)                          \
    extern "C" void * factoryname ## _create ();                        \
    extern "C" void factoryname ## _destroy (void *obj);                \
    extern "C" int factoryname ## _getVersion (char *ver, int len);     \
    extern "C" int factoryname ## _getAbi (char *abi, int len);         \
    extern "C" int factoryname(void *api);
    
#define YARP_DEFINE_SHARED_CLASS(factoryname,classname)                 \
    void *factoryname ## _create () { return new classname; }           \
    void factoryname ## _destroy (void *obj) { delete (classname *)obj; } \
    int factoryname ## _getVersion (char *ver, int len) { return 0; }   \
    int factoryname ## _getAbi (char *abi, int len) { return 0; }       \
    int factoryname(void *api) { \
    struct yarp::os::SharedLibraryClassApi *sapi = (struct yarp::os::SharedLibraryClassApi *) api; \
    sapi->startCheck = VOCAB4('Y','A','R','P'); \
    sapi->create = factoryname ## _create; \
    sapi->destroy = factoryname ## _destroy; \
    sapi->getVersion = factoryname ## _getVersion; \
    sapi->getAbi = factoryname ## _getAbi; \
    for (int i=0; i<32; i++) { sapi->roomToGrow[i] = 0; }    \
    sapi->endCheck = VOCAB4('P','R','A','Y'); \
    }
    

template <class T>
class yarp::os::SharedLibraryClass {
private:
    void *deleter;
    T *content;
public:

    SharedLibraryClass(T *content, void *deleter ) : content(content),
                                                   deleter(deleter) {
    }

    ~SharedLibraryClass() {
        if (deleter!=0/*NULL*/) {
            SharedClassDeleterFunction del = 
                (SharedClassDeleterFunction)deleter;
            del(content);
        } else {
            delete content;
        }
        content = 0/*NULL*/;
    }

    T& getContent() {
        return *content;
    }
};


template <class T>
class yarp::os::SharedLibraryClassFactory {
private:
    SharedLibraryClassApi api;
public:
    SharedLibraryClassFactory(void *factory) {
        api.startCheck = 0;
        SharedClassFactoryFunction fn = (SharedClassFactoryFunction)factory;
        fn(&api);
    }

    T *create() {
        if (api.startCheck!=VOCAB4('Y','A','R','P')) return 0/*NULL*/;
        if (api.endCheck!=VOCAB4('P','R','A','Y')) return 0/*NULL*/;
        return (T *)api.create();
    }

    void destroy(T *obj) {
        api.destroy(obj);
    }

    /**
     *
     * With DLLs compiled against different runtimes, it is important
     * to match creation and destruction methods.  The easiest way
     * to do this is to isolate creation and destruction of a 
     * particular class to a particular DLL.
     *
     */
    SharedLibraryClass<T> *wrap() {
        T *t = create();
        if (t==0/*NULL*/) return 0/*NULL*/;
        return new SharedLibraryClass<T>(t,(void*)api.destroy);
    }
};



#endif

