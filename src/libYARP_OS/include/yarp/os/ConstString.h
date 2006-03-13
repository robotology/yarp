#ifndef _YARP2_CONSTSTRING_
#define _YARP2_CONSTSTRING_

namespace yarp {
  namespace os {
    class ConstString;
  }
}

/**
 * A constant string.  The yarp::os library returns strings in this form,
 * to avoid forcing you to use the same string implementation
 * we do.
 */
class yarp::os::ConstString {
public:
  
  /**
   * Constructor.  Creates an empty string.
   */
  ConstString();

  /**
   * Constructor.  Stores a copy of the specified string.
   * @param str the string to copy
   */
  ConstString(const char *str);

  /**
   * Destructor.
   */
  ~ConstString();

  /**
   * Copy constructor.
   */
  ConstString(const ConstString& alt);
  
  /**
   * Accesses the character sequence stored in this object.
   */
  const char *c_str() const;

private:
  const ConstString& operator = (const ConstString& alt);

  void *implementation;
};

#endif

