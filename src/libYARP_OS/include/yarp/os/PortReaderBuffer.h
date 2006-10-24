// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_PORTREADERBUFFER_
#define _YARP2_PORTREADERBUFFER_

#include <yarp/os/PortReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/Thread.h>

namespace yarp {
    namespace os {
        template <class T> class PortReaderBuffer;
        template <class T> class TypedReaderCallback;
        template <class T> class TypedReader;
#ifndef DOXYGEN_SHOULD_SKIP_THIS
        template <class T> class TypedReaderThread;
#endif /*DOXYGEN_SHOULD_SKIP_THIS*/
    }
    class PortReaderBufferBase;
    class PortReaderBufferBaseCreator;
}


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
    virtual void onRead(T& datum) = 0;
};

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
     * Read the next available object from the port.  The method
     * does not drop older objects even if newer ones are available.
     * @param shouldWait true if the method should wait until an object
     * is available
     * @return A pointer to an object read from the port, or NULL if none
     * is available and waiting was not requested.  This object is owned
     * by the communication system and should not be deleted by the user.
     * The object is available to the user until the next call to 
     * one of the read methods, after which it should not be accessed again.
     */
    //virtual T *readStrict(bool shouldWait=true) {
    //return read(shouldWait,true);
    //}

    /**
     * Read the newest available object from the port.  The method
     * drops older objects if newer ones are available.
     * @param shouldWait true if the method should wait until an object 
     * is available
     * @return A pointer to an object read from the port, or NULL if none
     * is available and waiting was not requested.  This object is owned
     * by the communication system and should not be deleted by the user.
     * The object is available to the user until the next call to 
     * one of the read methods, after which it should not be accessed again.
     */
    //virtual T *readNewest(bool shouldWait=true) {
    //    return read(shouldWait,false);
    //}


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
     * Check how many messages are waiting to be read.
     * @return
     */
    virtual int getPendingReads() = 0;

    /**
     * Destructor.
     */
    virtual ~TypedReader() {}
};




#ifndef DOXYGEN_SHOULD_SKIP_THIS

class yarp::PortReaderBufferBaseCreator {
public:
    virtual ~PortReaderBufferBaseCreator() {}

    virtual yarp::os::PortReader *create() = 0;
};

class yarp::PortReaderBufferBase : public yarp::os::PortReader {
public:
    PortReaderBufferBase(unsigned int maxBuffer) : 
        maxBuffer(maxBuffer) {
        creator = 0; /*NULL*/
        init();
        allowReuse = true;
        prune = false;
    }

    void setCreator(PortReaderBufferBaseCreator *creator) {
        this->creator = creator;
    }

    virtual ~PortReaderBufferBase();

    virtual yarp::os::PortReader *create() {
        if (creator!=0 /*NULL*/) {
            return creator->create();
        }
        return 0 /*NULL*/;
    }

    void release(yarp::os::PortReader *completed);

    int check();

    virtual bool read(yarp::os::ConnectionReader& connection);

    void setAutoRelease(bool flag = true);

    void setPrune(bool flag = true) {
        prune = flag;
    }

    void setAllowReuse(bool flag = true) {
        allowReuse = flag;
    }

    yarp::os::PortReader *readBase();

    unsigned int getMaxBuffer() {
        return maxBuffer;
    }

	void attachBase(yarp::os::Port& port);

	bool isClosed();

protected:
    void init();

    PortReaderBufferBaseCreator *creator;
    unsigned int maxBuffer;
    bool prune;
    bool allowReuse;
    void *implementation;
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
                    callback->onRead(*(reader->lastRead()));
                }
            }
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
            private yarp::PortReaderBufferBaseCreator {
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
    }

    /**
     * Destructor.
     */
    virtual ~PortReaderBuffer() {
        // it would also help to close the port, so
        // that incoming inputs are interrupted
        if (reader!=0/*NULL*/) {
            delete reader;
            reader = 0/*NULL*/;
        }
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
        last = (T *)implementation.readBase();
        if (last!=0/*NULL*/) {
            if (autoDiscard) {
                // go up to date
                while (check()) {
                    //printf("Dropping something\n");
                    last = (T *)implementation.readBase();
                }
            }
        }
        return last;
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
            delete reader;
            reader = 0/*NULL*/;
        }
        reader = new TypedReaderThread<T>(*this,callback);
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

	bool isClosed() {
		return implementation.isClosed();
	}

private:
    yarp::PortReaderBufferBase implementation;
    bool autoDiscard;
    T *last;
    TypedReaderThread<T> *reader;
};



#endif
