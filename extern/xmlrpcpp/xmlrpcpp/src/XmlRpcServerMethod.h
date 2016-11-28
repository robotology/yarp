
#ifndef XMLRPCSERVERMETHOD_H
#define XMLRPCSERVERMETHOD_H
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
//
// Summary for YARP:
// Copyright: 2002, 2003 Chris Morley
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <string>
#endif

namespace YarpXmlRpc {

  // Representation of a parameter or result value
  class XmlRpcValue;

  // The XmlRpcServer processes client requests to call RPCs
  class XmlRpcServer;

  //! Abstract class representing a single RPC method
  class XmlRpcServerMethod {
  public:
    //! Constructor
    XmlRpcServerMethod(std::string const& name, XmlRpcServer* server = 0);
    //! Destructor
    virtual ~XmlRpcServerMethod();

    //! Returns the name of the method
    std::string& name() { return _name; }

    //! Execute the method. Subclasses must provide a definition for this method.
    virtual void execute(XmlRpcValue& params, XmlRpcValue& result) = 0;

    //! Returns a help string for the method.
    //! Subclasses should define this method if introspection is being used.
    virtual std::string help() { return std::string(); }

  protected:
    std::string _name;
    XmlRpcServer* _server;
  };
} // namespace YarpXmlRpc

#endif // _XMLRPCSERVERMETHOD_H_
