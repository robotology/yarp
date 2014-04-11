// Summary for YARP:
// Copyright: 2002, 2003 Chris Morley
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#include "XmlRpcServerMethod.h"
#include "XmlRpcServer.h"

namespace YarpXmlRpc {


  XmlRpcServerMethod::XmlRpcServerMethod(std::string const& name, XmlRpcServer* server)
  {
    _name = name;
    _server = server;
    if (_server) _server->addMethod(this);
  }

  XmlRpcServerMethod::~XmlRpcServerMethod()
  {
    if (_server) _server->removeMethod(this);
  }


} // namespace YarpXmlRpc
