/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_OS_PORTREADERBUFFER_H
#define YARP_OS_PORTREADERBUFFER_H

#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/LocalReader.h>
#include <yarp/os/Bottle.h>

#include <stdio.h>

namespace yarp {
    namespace os {
        template <class T> class PortReaderBuffer;
        template <class T> class TypedReaderCallback;
        template <class T> class TypedReader;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
        template <class T> class TypedReaderThread;
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
        namespace impl {
            class PortReaderBufferBase;
            class PortReaderBufferBaseCreator;
        }
    }
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
YARP_OS_API void typedReaderMissingCallback();
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/

/**
 * A callback for typed data from a port.  If you want to get your data
 * while leaving the port free to read more in the
 * background, you can create a callback that implements this interface,
 * and tell the port about it using BufferedPort::useCallback(callback)
 * or PortReaderBuffer::useCallback(callback)
 *
 */
template <class T>
class yarp::os::TypedReaderCallback {
public:
    /**
     * Destructor.
     */
    virtual ~TypedReaderCallback() {}

    /**
     * Callback method.
     * @param datum data read from a port
     */
    virtual void onRead(T& datum) {
        YARP_UNUSED(datum);
        typedReaderMissingCallback();
    }

    /**
     * Callback method.  Passes along source of callback.
     * By default, this calls the version of onRead that just takes a
     * datum.
     * @param datum data read from a port
     * @param reader the original port (or delegate object)
     */
    virtual void onRead(T& datum, const yarp::os::TypedReader<T>& reader) {
        YARP_UNUSED(reader);
        onRead(datum);
    }
};

#ifdef _MSC_VER
template class YARP_OS_API yarp::os::TypedReaderCallback<yarp::os::Bottle>;
#endif

/**
 * A base class for sources of typed data.  This could be a
 * BufferedPort or a PortReaderBuffer.
 */
template <class T>
class yarp::os::TypedReader {
public:
    /**
     * Call this to strictly keep all messages, or allow old ones
     * to be quietly dropped.  If you don't call this,
     * old messages will be quietly dropped.
     * @param strict True to keep all messages until they are read,
     * false to drop old messages when a new one comes in.
     */
    virtual void setStrict(bool strict = true) = 0;

    /**
     * Read an available object from the port.
     * @param shouldWait true if the method should wait until an object is available
     * @return A pointer to an object read from the port, or NULL if none
     * is available and waiting was not requested.  This object is owned
     * by the communication system and should not be deleted by the user.
     * The object is available to the user until the next call to
     * one of the read methods, after which it should not be accessed again.
     */
    virtual T *read(bool shouldWait = true) = 0;


    /**
     * Abort any read operation currently in progress.
     */
    virtual void interrupt() = 0;

    /**
     * Get the last data returned by read()
     * @return pointer to last data returned by read(), or NULL on failure.
     */
    virtual T *lastRead() = 0;

    /**
     * @return true if Port associated with this
     * reader has been closed
     */
    virtual bool isClosed() = 0;

    /**
     * Set an object whose onRead method will be called when data is
     * available.
     * @param callback the object whose onRead method will be called with data
     */
    virtual void useCallback(TypedReaderCallback<T>& callback) = 0;

    /**
     * Remove a callback set up with useCallback()
     */
    virtual void disableCallback() = 0;

    /**
     * Check how many messages are waiting to be read.
     * @return number of pending messages
     */
    virtual int getPendingReads() = 0;

    /**
     * Destructor.
     */
    virtual ~TypedReader() {}


    /**
     * Get name of port being read from
     * @return name of port
     */
    virtual ConstString getName() const = 0;


   /**
     * If a message is received that requires a reply, use this
     * handler.  No buffering happens.
     * @param reader the handler to use
     */
    virtual void setReplier(PortReader& reader) = 0;


    /**
     *
     * Take control of the last object read.
     * YARP will not reuse that object until it is explicitly released
     * by the user.  Be careful - if you acquire objects without
     * releasing, YARP will keep making new ones to store incoming
     * messages.  So you need to release all objects you acquire
     * eventually to avoid running out of memory.
     *
     * @return the handle to call release() with in order to give YARP
     * back control of the last object read.
     *
     */
    virtual void *acquire() = 0;


    /**
     *
     * Return control to YARP of an object previously taken control of
     * with the acquire() method.
     *
     * @param handle the pointer returned by acquire() when control of
     * the object was taken by the user.
     *
     */
    virtual void release(void *handle) = 0;


    /**
     *
     * Try to provide data periodically.  If no new data arrives
     * in a given period, repeat the last data received (if any).
     * Similarly, the port should not pass on data more frequently
     * than the given period.
     *
     * @param period target period in (fractional) seconds.
     *
     */
    virtual void setTargetPeriod(double period) = 0;
};




#ifndef DOXYGEN_SHOULD_SKIP_THIS

class YARP_OS_API yarp::os::impl::PortReaderBufferBaseCreator {
public:
    virtual ~PortReaderBufferBaseCreator();

    virtual yarp::os::PortReader *create() = 0;
};

class YARP_OS_API yarp::os::impl::PortReaderBufferBase : public yarp::os::PortReader
{
public:
    PortReaderBufferBase(unsigned int maxBuffer);
    virtual ~PortReaderBufferBase();

    void setCreator(PortReaderBufferBaseCreator *creator);

    void setReplier(yarp::os::PortReader& reader);

    void setPrune(bool flag = true);

    void setAllowReuse(bool flag = true);

    void setTargetPeriod(double period);

    yarp::os::ConstString getName() const;

    unsigned int getMaxBuffer();

    bool isClosed();

    void clear();

    virtual yarp::os::PortReader *create();

    void release(yarp::os::PortReader *completed);

    int check();

    virtual bool read(yarp::os::ConnectionReader& connection);

    yarp::os::PortReader *readBase(bool& missed, bool cleanup);

    void interrupt();

    void attachBase(yarp::os::Port& port);

    // direct writer-buffer to reader-buffer pointer sharing methods

    virtual bool acceptObjectBase(yarp::os::PortReader *obj,
                                  yarp::os::PortWriter *wrapper);

    virtual bool forgetObjectBase(yarp::os::PortReader *obj,
                                  yarp::os::PortWriter *wrapper);

    virtual bool getEnvelope(PortReader& envelope);

    // user takes control of the current read object
    void *acquire();

    // user gives back an object
    void release(void *key);

#ifndef YARP_NO_DEPRECATED
    YARP_DEPRECATED void setAutoRelease(bool flag = true);
#endif // YARP_NO_DEPRECATED

protected:
    void init();

    PortReaderBufferBaseCreator *creator;
    unsigned int maxBuffer;
    bool prune;
    bool allowReuse;
    void *implementation;
    yarp::os::PortReader *replier;
    double period;
    double last_recv;
};





template <class T>
class yarp::os::TypedReaderThread : public Thread {
public:
    TypedReader<T> *reader;
    TypedReaderCallback<T> *callback;

    TypedReaderThread() { reader = 0 /*NULL*/;  callback = 0 /*NULL*/; }

    TypedReaderThread(TypedReader<T>& reader,
                      TypedReaderCallback<T>& callback) {
        this->reader = &reader;
        this->callback = &callback;
        start(); // automatically starts running
    }

    virtual void run() {
        if (reader!=0/*NULL*/&&callback!=0/*NULL*/) {
            while (!isStopping()&&!reader->isClosed()) {
                if (reader->read()) {
                    callback->onRead(*(reader->lastRead()),
                                     *reader);
                }
            }
        }
    }

    virtual void onStop() {
        if (reader!=0/*NULL*/) {
            reader->interrupt();
        }
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


/**
 * Buffer incoming data to a port.
 * An instance of this class can be associated with a Port by calling
 * attach().  From then on data that arrives to the Port will
 * be passed to this buffer, to be picked up at the user's leisure
 * by calling check() and read().  "T" should be a PortReader
 * class, such as Bottle.
 */
template <class T>
class yarp::os::PortReaderBuffer :
    public yarp::os::TypedReader<T>,
            public yarp::os::LocalReader<T>,
            public yarp::os::impl::PortReaderBufferBaseCreator {
public:

    /**
     * Constructor.
     * @param maxBuffer Maximum number of buffers permitted (0 = no limit)
     */
    PortReaderBuffer(unsigned int maxBuffer = 0) :
        implementation(maxBuffer) {
        implementation.setCreator(this);
        last = 0; /*NULL*/
        setStrict(false);
        reader = 0 /*NULL*/;
        default_value = 0 /*NULL*/;
    }

    /**
     * Destructor.
     */
    virtual ~PortReaderBuffer() {
        detach();
    }

    void detach() {
        // it would also help to close the port, so
        // that incoming inputs are interrupted
        if (reader!=0/*NULL*/) {
            reader->stop();
            delete reader;
            reader = 0/*NULL*/;
        }
        if (default_value!=0/*NULL*/) {
            delete default_value;
            default_value = 0/*NULL*/;
        }
        last = 0/*NULL*/;
        implementation.clear();
    }

    // documented in TypedReader
    virtual void setStrict(bool strict = true) {
        autoDiscard = !strict;
        // do discard at earliest time possible
        implementation.setPrune(autoDiscard);
    }

    /**
     * Check if data is available.
     * @return true iff data is available (i.e. a call to read() will return
     * immediately and successfully)
     */
    bool check() {
        return implementation.check()>0;
    }

    virtual int getPendingReads() {
        return implementation.check();
    }


    // documented in TypedReader
    T *read(bool shouldWait=true) {
        if (!shouldWait) {
            if (!check()) {
                last = 0; /*NULL*/
                return last;
            }
        }
        bool missed = false;
        T *prev = last;
        last = (T *)implementation.readBase(missed,false);
        if (last!=0/*NULL*/) {
            if (autoDiscard) {
                // go up to date
                while (check()) {
                    //printf("Dropping something\n");
                    bool tmp;
                    last = (T *)implementation.readBase(tmp,true);
                }
            }
        }
        if (missed) {
            // we've been asked to enforce a period
            last = prev;
            if (last==0/*NULL*/) {
                if (default_value==0/*NULL*/) {
                    default_value = new T;
                }
                last = default_value;
            }
        }
        return last;
    }

    // documented in TypedReader
    void interrupt() {
        implementation.interrupt();
    }

    // documented in TypedReader
    T *lastRead() {
        return last;
    }

    /**
     * Attach this buffer to a particular port.  Data arriving to that
     * port will from now on be placed in this buffer.
     * @param port the port to attach to
     */
    void attach(Port& port) {
        //port.setReader(*this);
        implementation.attachBase(port);
    }

    void useCallback(TypedReaderCallback<T>& callback) {
        if (reader!=0/*NULL*/) {
            reader->stop();
            delete reader;
            reader = 0/*NULL*/;
        }
        reader = new TypedReaderThread<T>(*this,callback);
    }

    void disableCallback() {
        if (reader!=0/*NULL*/) {
            reader->stop();
            delete reader;
            reader = 0/*NULL*/;
        }
    }

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /**
     * Reads objects from a network connection.
     * This method is called by a port when data is received.
     * @param connection an interface to the network connection for reading
     * @return true iff the object is successfully read
     */
    virtual bool read(ConnectionReader& connection) {
        return implementation.read(connection);
    }

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/



    /**
     * Factory method.  New instances are created as needed to store incoming
     * data.  By default, this just uses the default contructor - override
     * this if you need to do something fancier (such as allocating
     * a shared memory space).
     *
     * @return new instance of the templated type.
     */
    virtual PortReader *create() {
        return new T;
    }

    void setReplier(PortReader& reader) {
        implementation.setReplier(reader);
    }

    virtual bool getEnvelope(PortReader& envelope) {
        return implementation.getEnvelope(envelope);
    }

    bool isClosed() {
        return implementation.isClosed();
    }

    virtual ConstString getName() const {
        return implementation.getName();
    }


    virtual bool acceptObject(T *obj,
                              PortWriter *wrapper) {
        return implementation.acceptObjectBase(obj,wrapper);
    }

    virtual bool forgetObject(T *obj,
                              yarp::os::PortWriter *wrapper) {
        return implementation.forgetObjectBase(obj,wrapper);
    }


    virtual void *acquire() {
        return implementation.acquire();
    }

    virtual void release(void *handle) {
        implementation.release(handle);
    }

    virtual void setTargetPeriod(double period) {
        implementation.setTargetPeriod(period);
    }

private:
    yarp::os::impl::PortReaderBufferBase implementation;
    bool autoDiscard;
    T *last;
    T *default_value;
    TypedReaderThread<T> *reader;
};


#endif // YARP_OS_PORTREADERBUFFER_H
