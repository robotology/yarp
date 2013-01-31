// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef _YARP2_SHAREDLIBRARYCLASS_
#define _YARP2_SHAREDLIBRARYCLASS_

#include <yarp/os/SharedLibrary.h>
#include <yarp/conf/system.h>

#include <string.h>

namespace yarp {
    namespace os {
        template <class T>
        class SharedLibraryClass;

        class SharedLibraryFactory;

        template <class T>
        class SharedLibraryClassFactory;

        struct SharedLibraryClassApi;

        extern "C" {
            typedef int (*SharedClassFactoryFunction)(void *ptr,int len);
            typedef void (*SharedClassDeleterFunction)(void *ptr);
        }
  }
}

#include <yarp/os/Vocab.h>

// Be careful loading C++ classes from DLLs.  Generally you
// need an exact or very close match between compilers used
// to compile those DLLs and your own code.

#define YARP_SHAREDLIBRARYCLASSAPI_PADDING (30-2*(YARP_POINTER_SIZE/4))
#include <yarp/os/begin_pack_for_net.h>
extern "C" {
    struct yarp::os::SharedLibraryClassApi {
    public:
        NetInt32 startCheck; // should be 'Y' 'A' 'R' 'P'
        NetInt32 structureSize;
        NetInt32 systemVersion;
        void *(*create)();
        void (*destroy)(void *obj);
        int (*getVersion)(char *ver, int len);
        int (*getAbi)(char *abi, int len);
        int (*getClassName)(char *name, int len);
        int (*getBaseClassName)(char *name, int len);
        NetInt32 roomToGrow[YARP_SHAREDLIBRARYCLASSAPI_PADDING];
        NetInt32 endCheck;   // should be 'P' 'L' 'U' 'G'
    };
}
#include <yarp/os/end_pack_for_net.h>

#define YARP_SHARED_CLASS_FN extern "C" YARP_EXPORT

#define YARP_DEFINE_SHARED_SUBCLASS(factoryname,classname,basename)       \
    YARP_SHARED_CLASS_FN void *factoryname ## _create () { return (basename *)new classname; } \
    YARP_SHARED_CLASS_FN void factoryname ## _destroy (void *obj) { delete (classname *)obj; } \
    YARP_SHARED_CLASS_FN int factoryname ## _getVersion (char *ver, int len) { return 0; }   \
    YARP_SHARED_CLASS_FN int factoryname ## _getAbi (char *abi, int len) { return 0; }       \
    YARP_SHARED_CLASS_FN int factoryname ## _getClassName (char *name, int len) { char cname[] = # classname; strncpy(name,cname,len); return strlen(cname)+1; } \
    YARP_SHARED_CLASS_FN int factoryname ## _getBaseClassName (char *name, int len) { char cname[] = # basename; strncpy(name,cname,len); return strlen(cname)+1; } \
    YARP_SHARED_CLASS_FN int factoryname(void *api,int len) { \
    struct yarp::os::SharedLibraryClassApi *sapi = (struct yarp::os::SharedLibraryClassApi *) api; \
    if (len<(int)sizeof(yarp::os::SharedLibraryClassApi)) return -1;    \
    sapi->startCheck = VOCAB4('Y','A','R','P'); \
    sapi->structureSize = sizeof(yarp::os::SharedLibraryClassApi);  \
    sapi->systemVersion = 2; \
    sapi->create = factoryname ## _create; \
    sapi->destroy = factoryname ## _destroy; \
    sapi->getVersion = factoryname ## _getVersion; \
    sapi->getAbi = factoryname ## _getAbi; \
    sapi->getClassName = factoryname ## _getClassName; \
    sapi->getBaseClassName = factoryname ## _getBaseClassName; \
    for (int i=0; i<YARP_SHAREDLIBRARYCLASSAPI_PADDING; i++) { sapi->roomToGrow[i] = 0; } \
    sapi->endCheck = VOCAB4('P','L','U','G');        \
    return sapi->startCheck;                    \
    }

#define YARP_DEFAULT_FACTORY_NAME "yarp_default_factory"
#define YARP_DEFINE_DEFAULT_SHARED_CLASS(classname) YARP_DEFINE_SHARED_SUBCLASS(yarp_default_factory,classname,classname)
#define YARP_DEFINE_SHARED_CLASS(factoryname,classname) YARP_DEFINE_SHARED_SUBCLASS(factoryname,classname,classname)


class YARP_OS_API yarp::os::SharedLibraryFactory {
private:
    SharedLibrary lib;
    int status;
    SharedLibraryClassApi api;
    int returnValue;
    int rct;
    ConstString name;
public:
    enum {
        STATUS_NONE,
        STATUS_OK = VOCAB2('o','k'),
        STATUS_LIBRARY_NOT_LOADED = VOCAB4('l','o','a','d'),
        STATUS_FACTORY_NOT_FOUND = VOCAB4('f','a','c','t'),
        STATUS_FACTORY_NOT_FUNCTIONAL = VOCAB3('r','u','n'),
    };

    SharedLibraryFactory() {
        api.startCheck = 0;
        status = STATUS_NONE;
        rct = 0;
        returnValue = 0;
	}

    SharedLibraryFactory(const char *dll_name, const char *fn_name = 0/*NULL*/) {
        rct = 0;
        returnValue = 0;
		open(dll_name,fn_name);
	}

    bool open(const char *dll_name, const char *fn_name = 0/*NULL*/) {
        returnValue = 0;
        name = "";
        status = STATUS_NONE;
        api.startCheck = 0;
        if (!lib.open(dll_name)) {
            //fprintf(stderr,"Failed to open library %s\n", dll_name);
            status = STATUS_LIBRARY_NOT_LOADED;
            return false;
        }
        void *fn = lib.getSymbol((fn_name!=0/*NULL*/)?fn_name:YARP_DEFAULT_FACTORY_NAME);
        if (fn==0/*NULL*/) {
            lib.close();
            status = STATUS_FACTORY_NOT_FOUND;
            return false;
        }
        useFactoryFunction(fn);
        if (!isValid()) {
            status = STATUS_FACTORY_NOT_FUNCTIONAL;
            return false;
        }
        status = STATUS_OK;
        name = dll_name;
        return true;
    }

    bool useFactoryFunction(void *factory) {
        api.startCheck = 0;
        if (factory==0/*NULL*/) return false;
        SharedClassFactoryFunction fn = (SharedClassFactoryFunction)factory;
        isValid();
        returnValue = fn(&api,sizeof(SharedLibraryClassApi));
        return isValid();
    }

	bool isValid() {
        if (returnValue!=VOCAB4('Y','A','R','P')) return false;
	    if (api.startCheck!=VOCAB4('Y','A','R','P')) return false;
	    if (api.structureSize!=sizeof(SharedLibraryClassApi)) return false;
	    if (api.systemVersion!=2) return false;
        if (api.endCheck!=VOCAB4('P','L','U','G')) return false;
        return true;
    }

    int getStatus() {
        return status;
    }

    void *getDestroyFn() {
        if (!isValid()) return 0/*NULL*/;
        return (void *)api.destroy;
    }

    const SharedLibraryClassApi& getApi() const {
        return api;
    }

    int getReferenceCount() const {
        return rct;
    }

    int addRef() {
        rct++;
        return rct;
    }

    int removeRef() {
        rct--;
        return rct;
    }

    ConstString getName() const {
        return name;
    }
};


template <class T>
class yarp::os::SharedLibraryClassFactory : public SharedLibraryFactory {
public:
    SharedLibraryClassFactory() {
    }

    SharedLibraryClassFactory(const char *dll_name, const char *fn_name = 0/*NULL*/) : SharedLibraryFactory(dll_name,fn_name) {
    }

    T *create() {
        if (!isValid()) return 0/*NULL*/;
        return (T *)getApi().create();
    }

    void destroy(T *obj) {
        if (!isValid()) return;
        getApi().destroy(obj);
    }
};


    /**
     *
     * Container for objects created in a DLL.
     * With DLLs compiled against different runtimes, it may be important
     * to match creation and destruction methods.  The easiest way
     * to do this is to isolate creation and destruction of a
     * particular class to a particular DLL.
     *
     */
template <class T>
class yarp::os::SharedLibraryClass {
private:
    void *deleter;
    T *content;
public:

    SharedLibraryClass() {
        content = 0/*NULL*/;
    }

    SharedLibraryClass(SharedLibraryClassFactory<T>& factory) {
        content = 0/*NULL*/;
        open(factory);
    }

    bool open(SharedLibraryClassFactory<T>& factory) {
        content = factory.create();
        deleter = factory.getDestroyFn();
        return content!=0/*NULL*/;
    }

    virtual bool close() {
        if (content!=0/*NULL*/) {
            if (deleter!=0/*NULL*/) {
                SharedClassDeleterFunction del =
                    (SharedClassDeleterFunction)deleter;
                del(content);
            } else {
                delete content;
            }
        }
        content = 0/*NULL*/;
        return true;
    }

    virtual ~SharedLibraryClass() {
        close();
    }

    T& getContent() {
        return *content;
    }

    bool isValid() const {
        return content!=0/*NULL*/;
    }

    T& operator*() {
        return (*content);
    }

    T *operator->() {
        return (content);
    }
};


#endif

