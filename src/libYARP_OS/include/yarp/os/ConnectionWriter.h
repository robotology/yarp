#ifndef _YARP2_CONNECTIONWRITER_
#define _YARP2_CONNECTIONWRITER_

namespace yarp {
  namespace os {
    class ConnectionWriter;
    class ConnectionReader;
    class PortReader;
  }
}

/**
 * An interface for writing to a network connection.
 *
 * @see Port, PortWriter, ConnectionReader
 */
class yarp::os::ConnectionWriter {
public:

  /**
   * Destructor.
   */
  virtual ~ConnectionWriter() {}

  /**
   * Send a block of data to the network connection.
   * Since communication may not happen immediately, or may
   * happen multiple times, a copy of this data is made.
   * If you know the block of data will remain valid,
   * and it is a large block of data,
   * please call ConnectionWriter::appendExternalBlock instead.
   * @param data the start of the data block
   * @param len  the length of the data block
   */
  virtual void appendBlock(const char *data, int len) = 0;

  /**
   * Send a representation of an integer to the network connection.
   * @param data the integer to send
   */
  virtual void appendInt(int data) = 0;

  /**
   * Send a character sequence to the network connection.
   * @param str the character sequence to send
   * @param terminate the terminating character to use
   */
  virtual void appendString(const char *str, int terminate = '\n') = 0;

  /**
   * Send a block of data to the network connection, without making a copy.
   * If you are not confident that the block of data will remain valid
   * throughout transmission, 
   * then call ConnectionWriter::appendBlock instead.
   * @param data the start of the data block
   * @param len  the length of the data block
   */
  virtual void appendExternalBlock(const char *data, int len) = 0;

  /**
   * Check if the connection is text mode.  If it is, you are
   * encouraged (but by no means required) to use a human-readable
   * representation of your data structure.  
   * @return true if the connection is text mode (as opposed to binary)
   */
  virtual bool isTextMode() = 0;

  /**
   * If you can easily determine how many blocks there are in a message, 
   * call this first, before anything else.  
   * This may improve efficiency in some situations.
   * @param argc Number of blocks
   * @param argv An array of integers, giving the length of each block
   */
  virtual void declareSizes(int argc, int *argv) = 0;

  /**
   * This sets a handler to deal with replies to the message.  The
   * handler will be called once per connection. There will be
   * problems for connections using carriers that don't support replies.
   * @param reader the object that handles replies.
   */
  virtual void setReplyHandler(PortReader& reader) = 0;
};

#endif
