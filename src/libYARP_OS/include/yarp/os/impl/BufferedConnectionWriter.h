/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_BUFFEREDCONNECTIONWRITER
#define YARP2_BUFFEREDCONNECTIONWRITER

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/SizedWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/NetInt64.h>

#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdlib.h>

namespace yarp {
    namespace os {
        namespace impl {
            class BufferedConnectionWriter;
            class ConnectionRecorder;
            using yarp::os::ConnectionWriter;
            using yarp::os::ConnectionReader;
        }
    }
    using os::ConnectionWriter;
    using os::ConnectionReader;
}

/**
 *
 * When allocating space to store serialized data, we start off with
 * a block of this size.  It will be resized as necessary.
 * Data can optionally have a header, serialized separately.
 *
 */
#define BUFFERED_CONNECTION_INITIAL_POOL_SIZE (1024)

/**
 *
 * A helper for creating cached object descriptions.  When a object is
 * to be sent from one port to another, and we have multiple
 * connections but don't want to serialize the object multiple times,
 * a BufferedConnectionWriter is useful. It will create buffer space
 * for any parts serialized on the fly, and keep track of any external
 * blocks (e.g. arrays of pixel data) that we want to include verbatim
 * in the message.  This class plays no role in management of the 
 * lifecycle of external blocks (e.g. when they are created/destroyed,
 * or when they may change in value). If you use external blocks, be
 * sure to pay attention to onCompletion() events on your object.
 *
 */
class YARP_OS_impl_API yarp::os::impl::BufferedConnectionWriter : public ConnectionWriter, public SizedWriter {
public:

    /**
     *
     * Constructor.
     *
     * @param textMode suggest that the object be serialized in a human
     * readable format.  BufferedConnectionWriter simply passes this flag
     * along to read/write methods, it takes on action on it.
     *
     * @param bareMode suggest that the object be serialized with the
     * assumption that all type information is known by recipient.
     * BufferedConnectionWriter simply passes this flag along to read/write 
     * methods, it takes on action on it.
     *
     */
    BufferedConnectionWriter(bool textMode = false,
                             bool bareMode = false) : textMode(textMode), bareMode(bareMode)
        {
        reader = YARP_NULLPTR;
        target = &lst;
        target_used = &lst_used;
        ref = YARP_NULLPTR;
        initialPoolSize = BUFFERED_CONNECTION_INITIAL_POOL_SIZE;
        stopPool();
        shouldDrop = false;
        convertTextModePending = false;
        lst_used = 0;
        header_used = 0;
    }

    /**
     *
     * Destructor.
     *
     */
    virtual ~BufferedConnectionWriter() {
        clear();
    }

    /**
     *
     * Completely clear the writer and start afresh.
     *
     * @param textMode see parameter to constructor for details
     *
     */
    void reset(bool textMode) {
        this->textMode = textMode;
        clear();
        reader = YARP_NULLPTR;
        ref = YARP_NULLPTR;
        convertTextModePending = false;
    }

    /**
     *
     * Tell the writer that we will be serializing a new object, but to
     * keep any cached buffers that already exist.  If the structure
     * of the new object matches that of what came before, the buffers
     * will be reused without any new memory allocation being necessary.
     * If the structure differs, memory allocation may be needed.
     *
     */
    void restart();

    /**
     *
     * Clear all cached data.
     *
     */
    virtual void clear() {
        target = &lst;
        target_used = &lst_used;

        size_t i;
        for (i=0; i<lst.size(); i++) {
            delete lst[i];
        }
        lst.clear();
        for (i=0; i<header.size(); i++) {
            delete header[i];
        }
        header.clear();
        stopPool();
        lst_used = 0;
        header_used = 0;
    }

    /**
     *
     * Add the specified bytes to the current pool buffer.
     * The pool buffer is a place to concatenate small
     * blocks of data that are not being held externally.
     * This requires a memory copy, so for large blocks
     * of data consider adding these separately.
     * A pool buffer will be created if none already exists.
     *
     * @return true on success
     *
     */
    bool addPool(const yarp::os::Bytes& data);

    /**
     *
     * Stop adding to the current pool buffer. Any further calls to
     * addPool() for the current write will result in creation
     * of a new pool.
     *
     */
    void stopPool() {
        pool = YARP_NULLPTR;
        poolIndex = 0;
        poolLength = initialPoolSize;
        poolCount = 0;
    }

    /**
     *
     * Add the specified buffer to the list of buffers to be written.
     * If the copy flag is set, the data in the buffer is copied,
     * otherwise a reference to it is kept (be careful to keep the
     * buffer valid until all writes that use it are complete and
     * onComplete() has been called on the object being sent).
     *
     * @param data the buffer to add
     * @param copy whether the data should be copied, or a reference stored
     *
     */
    void push(const Bytes& data, bool copy);

    /**
     *
     * Add a buffer by recording a reference to it, without copying it.
     * Be careful, this is the opposite of what appendBlock(ptr,len)
     * does. Sorry about that.
     *
     * @param data the buffer to add
     *
     */
    virtual void appendBlock(const yarp::os::Bytes& data) {
        stopPool();
        push(data,false);
    }

    /**
     *
     * Add a buffer by copying its contents
     *
     * @param data the buffer to add
     *
     */
    virtual void appendBlockCopy(const Bytes& data) {
        push(data,true);
    }

    // defined by yarp::os::ConnectionWriter
    virtual void appendInt(int data) {
        NetInt32 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        push(b,true);
    }

    // defined by yarp::os::ConnectionWriter
    virtual void appendInt64(const YARP_INT64& data) {
        NetInt64 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        push(b,true);
    }

    // defined by yarp::os::ConnectionWriter
    virtual void appendDouble(double data) {
        NetFloat64 i = data;
        yarp::os::Bytes b((char*)(&i),sizeof(i));
        push(b,true);
    }

    virtual void appendStringBase(const ConstString& data) {
        yarp::os::Bytes b((char*)(data.c_str()),data.length()+1);
        push(b,true);
    }

    /**
     *
     * Send a string along with a carriage-return-line-feed sequence.
     * This is a convenience function used by old parts of yarp,
     * for telnet compatibility on sockets.
     *
     * @param data string to write, not including carriage-return-line-feed.
     *
     */
    virtual void appendLine(const ConstString& data) {
        yarp::os::Bytes b((char*)(data.c_str()),data.length());
        push(b,true);
        const char *eol = "\r\n"; // for windows compatibility
        yarp::os::Bytes beol((char*)eol,2);
        push(beol,true);
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool isTextMode() {
        return textMode;
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool isBareMode() {
        return bareMode;
    }

    // defined by yarp::os::SizedWriter
    bool write(ConnectionWriter& connection) {
        stopWrite();
        size_t i;
        for (i=0; i<header_used; i++) {
            yarp::os::ManagedBytes& b = *(header[i]);
            connection.appendBlock(b.get(),b.used());
        }
        for (i=0; i<lst_used; i++) {
            yarp::os::ManagedBytes& b = *(lst[i]);
            connection.appendBlock(b.get(),b.used());
        }
        return !connection.isError();
    }

    // defined by yarp::os::SizedWriter
    void write(OutputStream& os);

    /**
     *
     * @return the size of the message that will be sent, in bytes, including
     * the header and payload.
     *
     */
    virtual size_t dataSize() {
        size_t i;
        size_t len=0;
        for (i=0; i<header_used; i++) {
            yarp::os::ManagedBytes& b = *(header[i]);
            len += b.usedBytes().length();
        }
        for (i=0; i<lst_used; i++) {
            yarp::os::ManagedBytes& b = *(lst[i]);
            len += b.usedBytes().length();
        }
        return len;
    }

    // defined by yarp::os::SizedWriter
    virtual size_t length() {
        return header_used+lst_used;
    }

    // defined by yarp::os::SizedWriter
    virtual size_t headerLength() {
        return header_used;
    }

    /**
     *
     * @return number of cache buffers lying around, internal or external
     *
     */
    size_t bufferCount() const {
        return header.size() + lst.size();
    }

    // defined by yarp::os::SizedWriter
    virtual size_t length(size_t index) {
        if (index<header_used) {
            yarp::os::ManagedBytes& b = *(header[index]);
            return b.used();
        }
        yarp::os::ManagedBytes& b = *(lst[index-header.size()]);
        return b.used();
    }

    // defined by yarp::os::SizedWriter
    virtual const char *data(size_t index) {
        if (index<header_used) {
            yarp::os::ManagedBytes& b = *(header[index]);
            return (const char *)b.get();
        }
        yarp::os::ManagedBytes& b = *(lst[index-header.size()]);
        return (const char *)b.get();
    }

    /**
     *
     * @return the message serialized as a string
     *
     */
    ConstString toString();

    // defined by yarp::os::ConnectionWriter
    virtual void appendBlock(const char *data, size_t len) {
        appendBlockCopy(yarp::os::Bytes((char*)data,len));
    }

    // defined by yarp::os::ConnectionWriter
    virtual void appendString(const char *str, int terminate = '\n') {
        if (terminate=='\n') {
            appendLine(str);
        } else if (terminate==0) {
            appendStringBase(str);
        } else {
            ConstString s = str;
            str += terminate;
            appendBlockCopy(yarp::os::Bytes((char*)(s.c_str()),s.length()));
        }
    }

    // defined by yarp::os::ConnectionWriter
    virtual void appendExternalBlock(const char *data, size_t len) {
        appendBlock(yarp::os::Bytes((char*)data,len));
    }

    // defined by yarp::os::ConnectionWriter
    virtual void declareSizes(int argc, int *argv) {
        // this method is never called yet, so no point using it yet.
    }

    // defined by yarp::os::ConnectionWriter
    virtual void setReplyHandler(PortReader& reader) {
        this->reader = &reader;
    }

    // defined by yarp::os::SizedWriter
    virtual PortReader *getReplyHandler() {
        return reader;
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool convertTextMode();

    /**
     *
     * Switch to storing a header.  Buffers are tracked separately
     * for the header.
     *
     */
    void addToHeader() {
        stopPool();
        target = &header;
        target_used = &header_used;
    }

    // defined by yarp::os::SizedWriter
    virtual yarp::os::Portable *getReference() {
        return ref;
    }

    // defined by yarp::os::ConnectionWriter
    virtual void setReference(yarp::os::Portable *obj) {
        ref = obj;
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool isValid() {
        return true;
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool isActive() {
        return true;
    }

    // defined by yarp::os::ConnectionWriter
    virtual bool isError() {
        return false;  // output errors are of no significance at user level
    }

    // defined by yarp::os::ConnectionWriter
   virtual void requestDrop() {
        shouldDrop = true;
    }

    // defined by yarp::os::SizedWriter
    bool dropRequested() {
        return shouldDrop;
    }

    // defined by yarp::os::SizedWriter
    virtual void startWrite() {}

    // defined by yarp::os::SizedWriter
    virtual void stopWrite() {
        // convert, last thing, if requested
        applyConvertTextMode();
    }

    // defined by yarp::os::ConnectionWriter
    virtual SizedWriter *getBuffer() {
        return this;
    }

    /**
     *
     * Write message to a receiving object.  This is to simplify writing tests,
     * YARP does not use this internally.
     * @param obj object to write into
     * @return true on success
     *
     */
    bool write(PortReader& obj);

    /**
     *
     * Set a custom initial pool size, which affects the size of buffers
     * created for temporary data storage.  If this method is not called,
     * the default used is BUFFERED_CONNECTION_INITIAL_POOL_SIZE
     * @param size the initial buffer size (in bytes) to use
     *
     */
    void setInitialPoolSize(size_t size) {
        initialPoolSize = size;
    }

private:
    /**
     *
     * Do the work of converting a text mode message to binary,
     * if that has been requested. Conversion can only happen once
     * the full message is available, whereas conversion can be
     * requested at any time.
     *
     * @return true on success
     *
     */
    bool applyConvertTextMode();


    PlatformVector<yarp::os::ManagedBytes *> lst;    ///< buffers in payload
    PlatformVector<yarp::os::ManagedBytes *> header; ///< buffers in header
    PlatformVector<yarp::os::ManagedBytes *> *target;///< points to header or payload
    yarp::os::ManagedBytes *pool; ///< the pool buffer (in lst or header)
    size_t poolIndex;  ///< current offset into pool buffer
    size_t poolCount;  ///< number of pool buffers allocated
    size_t poolLength; ///< length of current pool buffer
    yarp::os::PortReader *reader; ///< reply handler, if any
    bool textMode;     ///< should we be writing in text mode
    bool bareMode;     ///< should we be writing without including type info
    bool convertTextModePending; ///< will we need to do an automatic textmode conversion
    yarp::os::Portable *ref; ///< object reference for when serialization can be skipped
    bool shouldDrop;   ///< should the connection drop after writes
    size_t lst_used;   ///< how many payload buffers are in use for the current message
    size_t header_used;///< how many header buffers are in use for the current message
    size_t *target_used;///< points to lst_used of header_used
    size_t initialPoolSize; ///< size of new pool buffers
};


/**
 * A helper for recording entire message/reply transactions
 */
class yarp::os::impl::ConnectionRecorder : public ConnectionReader,
            public ConnectionWriter, public yarp::os::PortWriter {
private:
    ConnectionReader *reader;
    ConnectionWriter *writer;
    BufferedConnectionWriter readerStore;
    BufferedConnectionWriter writerStore;
    bool writing;
    bool wrote;
    bool skipNextInt;
    yarp::os::Bottle blank;
public:
    ConnectionRecorder() {
        reader = YARP_NULLPTR;
        writing = false;
        wrote = false;
        skipNextInt = false;
    }

    /**
     *
     * Call this to wrap a specific ConnectionReader.
     *
     */
    void init(ConnectionReader *wrappedReader) {
        reader = wrappedReader;
        if (reader->isTextMode()) {
            reader->convertTextMode();
        }
        writing = false;
    }

    /**
     *
     * Call this when all reading/writing has been done.
     *
     */
    void fini() {
        if (writing) {
            if (writer->isTextMode()) {
                writer->convertTextMode();
            }
            writing = false;
        }
    }

    virtual bool expectBlock(const char *data, size_t len) {
        bool ok = reader->expectBlock(data,len);
        if (ok) {
            readerStore.appendBlock(data,len);
        }
        return ok;
    }

    virtual ConstString expectText(int terminatingChar) {
        ConstString str = reader->expectText(terminatingChar);
        readerStore.appendString(str.c_str(),terminatingChar);
        return str;
    }

    virtual int expectInt() {
        int x = reader->expectInt();
        if (!skipNextInt) {
            readerStore.appendInt(x);
        } else {
            skipNextInt = false;
        }
        return x;
    }

    virtual YARP_INT64 expectInt64() {
        YARP_INT64 x = reader->expectInt64();
        readerStore.appendInt64(x);
        return x;
    }

    virtual bool pushInt(int x) {
        bool ok = reader->pushInt(x);
        skipNextInt = skipNextInt || ok;
        return ok;
    }

    virtual double expectDouble() {
        double x = reader->expectDouble();
        readerStore.appendDouble(x);
        return x;
    }

    virtual bool isTextMode() {
        return false;
    }

    virtual bool isBareMode() {
        return false;
    }

    virtual bool convertTextMode() {
        return false;
    }

    virtual size_t getSize() {
        return reader->getSize();
    }


    virtual ConnectionWriter *getWriter() {
        writer = reader->getWriter();
        writing = true;
        wrote = true;
        return this;
    }

    virtual Portable *getReference() {
        return reader->getReference();
    }

    virtual Contact getRemoteContact() {
        return reader->getRemoteContact();
    }

    virtual Contact getLocalContact() {
        return reader->getLocalContact();
    }

    virtual bool isValid() {
        // shared
        if (writing) {
            return writer->isValid();
        }
        return reader->isValid();
    }

    virtual bool isActive() {
        // shared
        if (writing) {
            return writer->isActive();
        }
        return reader->isActive();
    }

    virtual bool isError() {
        // shared
        if (writing) {
            return writer->isError();
        }
        return reader->isError();
    }


    virtual void appendBlock(const char *data, size_t len) {
        writer->appendBlock(data,len);
        writerStore.appendBlock(data,len);
    }

    virtual void appendInt(int data) {
        writer->appendInt(data);
        writerStore.appendInt(data);
    }

    virtual void appendInt64(const YARP_INT64& data) {
        writer->appendInt64(data);
        writerStore.appendInt64(data);
    }

    virtual void appendDouble(double data) {
        writer->appendDouble(data);
        writerStore.appendDouble(data);
    }

    virtual void appendString(const char *str, int terminate) {
        writer->appendString(str,terminate);
        writerStore.appendString(str,terminate);
    }

    virtual void appendExternalBlock(const char *data, size_t len) {
        writer->appendExternalBlock(data,len);
        writerStore.appendExternalBlock(data,len);
    }

    virtual void declareSizes(int argc, int *argv) {
        writer->declareSizes(argc,argv);
    }

    virtual void setReplyHandler(PortReader& reader) {
        writer->setReplyHandler(reader);
    }

    virtual void setReference(Portable *obj) {
        writer->setReference(obj);
    }

    virtual bool write(ConnectionWriter& connection) {
        if (hasReply()) {
            connection.appendInt(BOTTLE_TAG_LIST); // nested structure
            connection.appendInt(3);               // with three elements
            connection.appendInt(BOTTLE_TAG_VOCAB);
            connection.appendInt(VOCAB3('r','p','c'));
            bool ok = readerStore.write(connection);
            if (ok) {
                writerStore.write(connection);
            }
            return ok;
        } else {
            return readerStore.write(connection);
        }
    }

    virtual void requestDrop() {
    }

    virtual yarp::os::Searchable& getConnectionModifiers() {
        return blank;
    }

    BufferedConnectionWriter& getMessage() { return readerStore; }
    BufferedConnectionWriter& getReply() { return writerStore; }
    bool hasReply() { return wrote; }
    virtual SizedWriter *getBuffer() { return YARP_NULLPTR; }

    virtual bool setSize(size_t len) {
        return reader->setSize(len);
    }
};



#endif
