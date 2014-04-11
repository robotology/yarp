
// Summary for YARP:
// Copyright: 2002, 2003 Chris Morley
// CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#include "XmlRpcClient.h"

#include "XmlRpc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace YarpXmlRpc;

// Static data
const char XmlRpcClient::REQUEST_BEGIN[] = 
  "<?xml version=\"1.0\"?>\r\n"
  "<methodCall><methodName>";
const char XmlRpcClient::REQUEST_END_METHODNAME[] = "</methodName>\r\n";
const char XmlRpcClient::PARAMS_TAG[] = "<params>";
const char XmlRpcClient::PARAMS_ETAG[] = "</params>";
const char XmlRpcClient::PARAM_TAG[] = "<param>";
const char XmlRpcClient::PARAM_ETAG[] =  "</param>";
const char XmlRpcClient::REQUEST_END[] = "</methodCall>\r\n";
const char XmlRpcClient::METHODRESPONSE_TAG[] = "<methodResponse>";
const char XmlRpcClient::FAULT_TAG[] = "<fault>";



XmlRpcClient::XmlRpcClient(const char* host, int port, const char* uri/*=0*/)
{
  XmlRpcUtil::log(1, "XmlRpcClient new client: host %s, port %d.", host, port);

  _host = host;
  _port = port;
  if (uri)
    _uri = uri;
  else
    _uri = "/RPC2";
  _connectionState = NO_CONNECTION;
  _executing = false;
  _eof = false;

  // Default to keeping the connection open until an explicit close is done
  setKeepOpen();
}


XmlRpcClient::~XmlRpcClient()
{
}

// Close the owned fd
void 
XmlRpcClient::close()
{
  XmlRpcUtil::log(4, "XmlRpcClient::close: fd %d.", getfd());
  _connectionState = NO_CONNECTION;
  XmlRpcSource::close();
}


// Clear the referenced flag even if exceptions or errors occur.
struct ClearFlagOnExit {
  ClearFlagOnExit(bool& flag) : _flag(flag) {}
  ~ClearFlagOnExit() { _flag = false; }
  bool& _flag;
};

// Execute the named procedure on the remote server.
// Params should be an array of the arguments for the method.
// Returns true if the request was sent and a result received (although the result
// might be a fault).
bool 
XmlRpcClient::execute(const char* method, XmlRpcValue const& params, XmlRpcValue& result)
{
  return false;
}

// XmlRpcSource interface implementation
// Handle server responses. Called by the event dispatcher during execute.
unsigned
XmlRpcClient::handleEvent(unsigned eventType)
{
  return 0;
}


// Create the socket connection to the server if necessary
bool 
XmlRpcClient::setupConnection()
{
  // If an error occurred last time through, or if the server closed the connection, close our end
  if ((_connectionState != NO_CONNECTION && _connectionState != IDLE) || _eof)
    close();

  _eof = false;
  if (_connectionState == NO_CONNECTION)
    if (! doConnect()) 
      return false;

  // Prepare to write the request
  _connectionState = WRITE_REQUEST;
  _bytesWritten = 0;

  return true;
}


// Connect to the xmlrpc server
bool 
XmlRpcClient::doConnect()
{
  return false;
}

// Encode the request to call the specified method with the specified parameters into xml
bool 
XmlRpcClient::generateRequest(const char* methodName, XmlRpcValue const& params)
{
  std::string body = REQUEST_BEGIN;
  body += methodName;
  body += REQUEST_END_METHODNAME;

  // If params is an array, each element is a separate parameter
  if (params.valid()) {
    body += PARAMS_TAG;
    if (params.getType() == XmlRpcValue::TypeArray)
    {
      for (int i=0; i<params.size(); ++i) {
        body += PARAM_TAG;
        body += params[i].toXml();
        body += PARAM_ETAG;
      }
    }
    else
    {
      body += PARAM_TAG;
      body += params.toXml();
      body += PARAM_ETAG;
    }
      
    body += PARAMS_ETAG;
  }
  body += REQUEST_END;

  std::string header = generateHeader(body);
  XmlRpcUtil::log(4, "XmlRpcClient::generateRequest: header is %d bytes, content-length is %d.", 
                  header.length(), body.length());

  _request = header + body;
  return true;
}

// Prepend http headers
std::string
XmlRpcClient::generateHeader(std::string const& body)
{
  std::string header = 
    "POST " + _uri + " HTTP/1.1\r\n"
    "User-Agent: ";
  header += XMLRPC_VERSION;
  header += "\r\nHost: ";
  header += _host;

  char buff[40];
  sprintf(buff,":%d\r\n", _port);

  header += buff;
  header += "Content-Type: text/xml\r\nContent-length: ";

  sprintf(buff,"%d\r\n\r\n", (int)body.size());

  return header + buff;
}

bool 
XmlRpcClient::writeRequest()
{
  return false;
}

void XmlRpcClient::reset() {
  _header = "";
  _response = "";
  _eof = false;
  _connectionState = READ_HEADER;
}

bool XmlRpcClient::read(const std::string& txt) {
  std::string got = txt;
  if (_connectionState==READ_HEADER) {
    readHeader(got);
    got = "";
  }
  if (_connectionState==READ_RESPONSE) {
    readResponse(got);
  }
  return (_connectionState == IDLE);
}

// Read the header from the response
bool 
XmlRpcClient::readHeader()
{
  return false;
}

// Read the header from the response
bool
XmlRpcClient::readHeader(const std::string& txt) {
  _header += txt;
  _eof = false;

  XmlRpcUtil::log(4, "XmlRpcClient::readHeader: client has read %d bytes", _header.length());

  char *hp = (char*)_header.c_str();  // Start of header
  char *ep = hp + _header.length();   // End of string
  char *bp = 0;                       // Start of body
  char *lp = 0;                       // Start of content-length value

  for (char *cp = hp; (bp == 0) && (cp < ep); ++cp) {
    if ((ep - cp > 16) && (strncasecmp(cp, "Content-length: ", 16) == 0))
      lp = cp + 16;
    else if ((ep - cp > 4) && (strncmp(cp, "\r\n\r\n", 4) == 0))
      bp = cp + 4;
    else if ((ep - cp > 2) && (strncmp(cp, "\n\n", 2) == 0))
      bp = cp + 2;
  }

  // If we haven't gotten the entire header yet, return (keep reading)
  if (bp == 0) {
    if (_eof)          // EOF in the middle of a response is an error
    {
      XmlRpcUtil::error("Error in XmlRpcClient::readHeader: EOF while reading header");
      return false;   // Close the connection
    }
    
    return true;  // Keep reading
  }

  // Decode content length
  if (lp == 0) {
    XmlRpcUtil::error("Error XmlRpcClient::readHeader: No Content-length specified");
    return false;   // We could try to figure it out by parsing as we read, but for now...
  }

  _contentLength = atoi(lp);
  if (_contentLength <= 0) {
    XmlRpcUtil::error("Error in XmlRpcClient::readHeader: Invalid Content-length specified (%d).", _contentLength);
    return false;
  }
  	
  XmlRpcUtil::log(4, "client read content length: %d", _contentLength);

  // Otherwise copy non-header data to response buffer and set state to read response.
  _response = bp;
  _header = "";   // should parse out any interesting bits from the header (connection, etc)...
  _connectionState = READ_RESPONSE;
  return true;    // Continue monitoring this source
}


    
bool 
XmlRpcClient::readResponse() {
  return false;
}

bool
XmlRpcClient::readResponse(const std::string& txt)
{
  _eof = false;

  // If we dont have the entire response yet, read available data
  if (int(_response.length()) < _contentLength) {
    _response += txt;

    // If we haven't gotten the entire _response yet, return (keep reading)
    if (int(_response.length()) < _contentLength) {
      if (_eof) {
        XmlRpcUtil::error("Error in XmlRpcClient::readResponse: EOF while reading response");
        return false;
      }
      return true;
    }
  }

  // Otherwise, parse and return the result
  XmlRpcUtil::log(3, "XmlRpcClient::readResponse (read %d bytes)", _response.length());
  XmlRpcUtil::log(5, "response:\n%s", _response.c_str());

  _connectionState = IDLE;

  return false;    // Stop monitoring this source (causes return from work)
}


// Convert the response xml into a result value
bool 
XmlRpcClient::parseResponse(XmlRpcValue& result)
{
  // Parse response xml into result
  int offset = 0;
  if ( ! XmlRpcUtil::findTag(METHODRESPONSE_TAG,_response,&offset)) {
    XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response - no methodResponse. Response:\n%s", _response.c_str());
    return false;
  }

  // Expect either <params><param>... or <fault>...
  if (((XmlRpcUtil::nextTagIs(PARAMS_TAG,_response,&offset) &&
	XmlRpcUtil::nextTagIs(PARAM_TAG,_response,&offset))) ||
      (XmlRpcUtil::nextTagIs(FAULT_TAG,_response,&offset) && (_isFault = true)))
  {
    if ( ! result.fromXml(_response, &offset)) {
      XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response value. Response:\n%s", _response.c_str());
      _response = "";
      return false;
    }
  } else {
    XmlRpcUtil::error("Error in XmlRpcClient::parseResponse: Invalid response - no param or fault tag. Response:\n%s", _response.c_str());
    _response = "";
    return false;
  }
      
  _response = "";
  return result.valid();
}

