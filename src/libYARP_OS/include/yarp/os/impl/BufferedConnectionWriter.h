/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_OS_IMPL_BUFFEREDCONNECTIONWRITER_H
#define YARP_OS_IMPL_BUFFEREDCONNECTIONWRITER_H

#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/SizedWriter.h>

#include <string>
#include <vector>


namespace yarp {
namespace os {

class Bytes;
class ManagedBytes;

namespace impl {

/*
 * When allocating space to store serialized data, we start off with
 * a block of this size.  It will be resized as necessary.
 * Data can optionally have a header, serialized separately.
 */
constexpr size_t BUFFERED_CONNECTION_INITIAL_POOL_SIZE = 1024;

/**
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
 */
class YARP_OS_impl_API BufferedConnectionWriter : public yarp::os::ConnectionWriter,
                                                  public yarp::os::SizedWriter
{
public:
    /**
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
     */
    BufferedConnectionWriter(bool textMode = false,
                             bool bareMode = false);

    /**
     * Destructor.
     */
    virtual ~BufferedConnectionWriter();

    /**
     * Completely clear the writer and start afresh.
     *
     * @param textMode see parameter to constructor for details
     */
    void reset(bool textMode);

    /**
     * Tell the writer that we will be serializing a new object, but to
     * keep any cached buffers that already exist.  If the structure
     * of the new object matches that of what came before, the buffers
     * will be reused without any new memory allocation being necessary.
     * If the structure differs, memory allocation may be needed.
     */
    void restart();

    /**
     * Clear all cached data.
     */
    virtual void clear() override;

    /**
     * Add the specified bytes to the current pool buffer.
     * The pool buffer is a place to concatenate small
     * blocks of data that are not being held externally.
     * This requires a memory copy, so for large blocks
     * of data consider adding these separately.
     * A pool buffer will be created if none already exists.
     *
     * @return true on success
     */
    bool addPool(const yarp::os::Bytes& data);

    /**
     * Stop adding to the current pool buffer. Any further calls to
     * addPool() for the current write will result in creation
     * of a new pool.
     */
    void stopPool();

    /**
     * Add the specified buffer to the list of buffers to be written.
     * If the copy flag is set, the data in the buffer is copied,
     * otherwise a reference to it is kept (be careful to keep the
     * buffer valid until all writes that use it are complete and
     * onComplete() has been called on the object being sent).
     *
     * @param data the buffer to add
     * @param copy whether the data should be copied, or a reference stored
     */
    void push(const Bytes& data, bool copy);

    // defined by yarp::os::ConnectionWriter
    virtual bool isTextMode() override;
    virtual bool isBareMode() override;
    virtual bool convertTextMode() override;
    virtual void declareSizes(int argc, int* argv) override; // FIXME Remove?
    virtual void setReplyHandler(PortReader& reader) override;
    virtual void appendInt8(std::int8_t data) override;
    virtual void appendInt16(std::int16_t data) override;
    virtual void appendInt32(std::int32_t data) override;
    virtual void appendInt64(std::int64_t data) override;
    virtual void appendFloat32(yarp::conf::float32_t data) override;
    virtual void appendFloat64(yarp::conf::float64_t data) override;
    virtual void appendBlock(const char* data, size_t len) override;
    virtual void appendString(const char* str, int terminate = '\n') override;
    virtual void appendExternalBlock(const char* data, size_t len) override;

    /**
     * Add a buffer by recording a reference to it, without copying it.
     * Be careful, this is the opposite of what appendBlock(ptr, len)
     * does. Sorry about that.
     *
     * @param data the buffer to add
     */
    virtual void appendBlock(const yarp::os::Bytes& data);

    /**
     * Add a buffer by copying its contents
     *
     * @param data the buffer to add
     */
    virtual void appendBlockCopy(const Bytes& data);

    virtual void appendStringBase(const std::string& data);

    /**
     * Send a string along with a carriage-return-line-feed sequence.
     * This is a convenience function used by old parts of yarp,
     * for telnet compatibility on sockets.
     *
     * @param data string to write, not including carriage-return-line-feed.
     */
    virtual void appendLine(const std::string& data);


    // defined by yarp::os::SizedWriter
    virtual size_t length() override;
    virtual size_t headerLength() override;
    virtual size_t length(size_t index) override;
    virtual const char* data(size_t index) override;
    virtual bool write(ConnectionWriter& connection) override;
    virtual void write(OutputStream& os) override;

    /**
     * Write message to a receiving object.  This is to simplify writing tests,
     * YARP does not use this internally.
     * @param obj object to write into
     * @return true on success
     */
    bool write(PortReader& obj);

    /**
     * @return the size of the message that will be sent, in bytes, including
     * the header and payload.
     */
    virtual size_t dataSize();

    size_t bufferCount() const;


    // defined by yarp::os::SizedWriter
    virtual PortReader* getReplyHandler() override;

    /**
     * Switch to storing a header.  Buffers are tracked separately
     * for the header.
     */
    void addToHeader();


    // defined by yarp::os::SizedWriter
    virtual yarp::os::Portable* getReference() override;


    // defined by yarp::os::ConnectionWriter
    virtual void setReference(yarp::os::Portable* obj) override;
    virtual bool isValid() override;
    virtual bool isActive() override;
    virtual bool isError() override;
    virtual void requestDrop() override;

    // defined by yarp::os::SizedWriter
    virtual bool dropRequested() override;
    virtual void startWrite() override;
    virtual void stopWrite() override;

    // defined by yarp::os::ConnectionWriter
    virtual SizedWriter* getBuffer() override;

    /**
     * Set a custom initial pool size, which affects the size of buffers
     * created for temporary data storage.  If this method is not called,
     * the default used is BUFFERED_CONNECTION_INITIAL_POOL_SIZE
     * @param size the initial buffer size (in bytes) to use
     */
    void setInitialPoolSize(size_t size);


    /**
     * @return the message serialized as a string
     */
    std::string toString();


private:
    /**
     * Do the work of converting a text mode message to binary,
     * if that has been requested. Conversion can only happen once
     * the full message is available, whereas conversion can be
     * requested at any time.
     *
     * @return true on success
     */
    bool applyConvertTextMode();


    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<yarp::os::ManagedBytes*>) lst; ///< buffers in payload
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<yarp::os::ManagedBytes*>) header; ///< buffers in header
    YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::vector<yarp::os::ManagedBytes*>*) target; ///< points to header or payload
    yarp::os::ManagedBytes* pool; ///< the pool buffer (in lst or header)
    size_t poolIndex; ///< current offset into pool buffer
    size_t poolCount; ///< number of pool buffers allocated
    size_t poolLength; ///< length of current pool buffer
    yarp::os::PortReader* reader; ///< reply handler, if any
    bool textMode; ///< should we be writing in text mode
    bool bareMode; ///< should we be writing without including type info
    bool convertTextModePending; ///< will we need to do an automatic textmode conversion
    yarp::os::Portable* ref; ///< object reference for when serialization can be skipped
    bool shouldDrop; ///< should the connection drop after writes
    size_t lst_used; ///< how many payload buffers are in use for the current message
    size_t header_used; ///< how many header buffers are in use for the current message
    size_t* target_used; ///< points to lst_used of header_used
    size_t initialPoolSize; ///< size of new pool buffers
};


} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_BUFFEREDCONNECTIONWRITER_H
