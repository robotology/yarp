// Summary for YARP:
// Copyright: 2002, 2003 Chris Morley
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#include "XmlRpcSource.h"
#include "XmlRpcUtil.h"

namespace YarpXmlRpc {


  XmlRpcSource::XmlRpcSource(int fd /*= -1*/, bool deleteOnClose /*= false*/) 
    : _fd(fd), _deleteOnClose(deleteOnClose), _keepOpen(false)
  {
  }

  XmlRpcSource::~XmlRpcSource()
  {
  }


  void
  XmlRpcSource::close()
  {
    if (_deleteOnClose) {
      XmlRpcUtil::log(2,"XmlRpcSource::close: deleting this");
      _deleteOnClose = false;
      delete this;
    }
  }

} // namespace YarpXmlRpc
