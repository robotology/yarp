#include "XmlRpcStream.h"
#include "XmlRpcValue.h"

#include <yarp/os/Bottle.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace XmlRpc;
using namespace std;


Value toValue(XmlRpcValue& v) {
  int t = v.getType();
  switch (t) {
  case XmlRpcValue::TypeInt:
    return Value((int)v);
    break;
  case XmlRpcValue::TypeDouble:
    return Value((double)v);
    break;
  case XmlRpcValue::TypeString:
    return Value(((string)v).c_str());
    break;
  case XmlRpcValue::TypeArray:
    {
      Value vbot;
      Bottle *bot = vbot.asList();
      for (int i=0; i<v.size(); i++) {
	XmlRpcValue& v2 = v[i];
	if (v2.getType()!=XmlRpcValue::TypeInvalid) {
	  bot->add(toValue(v2));
	}
      }
      return vbot;
    }
    break;
  case XmlRpcValue::TypeStruct:
    {
      Value vbot;
      Bottle *bot = vbot.asList();
      XmlRpcValue::ValueStruct& vals = v;
      for (XmlRpcValue::ValueStruct::iterator it = vals.begin();
	   it!= vals.end();
	   it++) {
	XmlRpcValue& v2 = it->second;
	Bottle& sub = bot->addList();
	sub.addString(it->first.c_str());
	if (v2.getType()!=XmlRpcValue::TypeInvalid) {
	  sub.add(toValue(v2));
	}
      }
      return vbot;
    }
    break;
  case XmlRpcValue::TypeInvalid:
    return Value::getNullValue();
    break;
  }
  //printf("Skipping %d\n", t);
  return Value("(type not supported yet out of laziness)");
}

int XmlRpcStream::read(const Bytes& b) {
  int result = sis.read(b);
  if (result>0) {
    //printf("RETURNING %d bytes\n", result);
    return result;
  }
  if (result==0) {
    //printf("Reading...\n");
    bool ok = false;
    if (sender) {
      client.reset();
    } else {
      server.reset();
    }
    if (firstRound) {
      if (sender) {
	client.read("POST /RP");
      } else {
	server.read("POST /RP");
      }
      firstRound = false;
    }
    char buf[1000];
    Bytes bytes(buf,sizeof(buf));
    while (!ok) {
      int result2 = delegate->getInputStream().partialRead(bytes);
      if (result2<=0) {
	return result2;
      }
      string s(buf,result2);
      //printf("Giving %s to parser\n", s.c_str());
      if (sender) {
	ok = client.read(s);
      } else {
	ok = server.read(s);
      }
      if (ok) {
	//printf("got a block!\n");
	XmlRpcValue xresult;
	std::string prefix = "";
	if (sender) {
	  client.parseResponse(xresult);
	} else {
	  prefix = "d\n";
	  prefix += server.parseRequest(xresult);
	  prefix += " ";
	}
	//printf("xmlrpc block is %s\n", xresult.toXml().c_str());
	Value v = toValue(xresult);
	if (!v.isNull()) {
	  sis.reset((prefix + v.toString().c_str() + "\n").c_str());
	} else {
	  sis.reset((prefix + "\n").c_str());
	}
	//printf("String version is %s\n", sis.toString().c_str());
	result = sis.read(b);
	break;
      }
    }
  }
  //printf("RETURNING %d bytes\n", result);
  return (result>0)?result:-1;
}


void XmlRpcStream::write(const Bytes& b) {
  delegate->getOutputStream().write(b);
}
