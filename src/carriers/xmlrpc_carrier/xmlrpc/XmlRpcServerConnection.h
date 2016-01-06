#ifndef XMLRPCSERVERCONNECTION_H
#define XMLRPCSERVERCONNECTION_H
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

#include "XmlRpcValue.h"
#include "XmlRpcSource.h"

namespace YarpXmlRpc {


  // The server waits for client connections and provides methods
  class XmlRpcServer;
  class XmlRpcServerMethod;

  //! A class to handle XML RPC requests from a particular client
  class XmlRpcServerConnection : public XmlRpcSource {
  public:
    // Static data
    static const char METHODNAME_TAG[];
    static const char PARAMS_TAG[];
    static const char PARAMS_ETAG[];
    static const char PARAM_TAG[];
    static const char PARAM_ETAG[];

    static const std::string SYSTEM_MULTICALL;
    static const std::string METHODNAME;
    static const std::string PARAMS;

    static const std::string FAULTCODE;
    static const std::string FAULTSTRING;

    //! Constructor
    XmlRpcServerConnection(int fd, XmlRpcServer* server, bool deleteOnClose = false);
    //! Destructor
    virtual ~XmlRpcServerConnection();

    // XmlRpcSource interface implementation
    //! Handle IO on the client connection socket.
    //!   @param eventType Type of IO event that occurred. @see XmlRpcDispatch::EventType.
    virtual unsigned handleEvent(unsigned eventType);

    bool readHeader();
    bool readRequest();

    void reset();
    bool read(const std::string& txt);
    bool readHeader(const std::string& txt);
    bool readRequest(const std::string& txt);
    // Parse the methodName and parameters from the request.
    std::string parseRequest(XmlRpcValue& params);

    // Construct a response from the result XML.
    void generateResponse(std::string const& resultXml);
    std::string getResponse() { return _response; }

  protected:

    bool writeResponse();

    // Parses the request, runs the method, generates the response xml.
    virtual void executeRequest();

    // Execute a named method with the specified params.
    bool executeMethod(const std::string& methodName, XmlRpcValue& params, XmlRpcValue& result);

    // Execute multiple calls and return the results in an array.
    bool executeMulticall(const std::string& methodName, XmlRpcValue& params, XmlRpcValue& result);

    void generateFaultResponse(std::string const& msg, int errorCode = -1);
    std::string generateHeader(std::string const& body);


    // The XmlRpc server that accepted this connection
    XmlRpcServer* _server;

    // Possible IO states for the connection
    enum ServerConnectionState { READ_HEADER, READ_REQUEST, WRITE_RESPONSE };
    ServerConnectionState _connectionState;

    // Request headers
    std::string _header;

    // Number of bytes expected in the request body (parsed from header)
    int _contentLength;

    // Request body
    std::string _request;

    // Response
    std::string _response;

    // Number of bytes of the response written so far
    int _bytesWritten;

    // Whether to keep the current client connection open for further requests
    bool _keepAlive;
  };
} // namespace YarpXmlRpc

#endif // _XMLRPCSERVERCONNECTION_H_
