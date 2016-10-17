
#ifndef XMLRPCEXCEPTION_H
#define XMLRPCEXCEPTION_H
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
// Summary for YARP:
// Copyright: 2002, 2003 Chris Morley
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
//
#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif


namespace YarpXmlRpc {

  //! A class representing an error.
  //! If server methods throw this exception, a fault response is returned
  //! to the client.
  class XmlRpcException {
  public:
    //! Constructor
    //!   @param message  A descriptive error message
    //!   @param code     An integer error code
    XmlRpcException(const std::string& message, int code=-1) :
        _message(message), _code(code) {}

    //! Return the error message.
    const std::string& getMessage() const { return _message; }

    //! Return the error code.
    int getCode() const { return _code; }

  private:
    std::string _message;
    int _code;
  };

}

#endif	// _XMLRPCEXCEPTION_H_
