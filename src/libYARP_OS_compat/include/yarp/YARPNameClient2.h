
#if !defined __NAME_CLIENT2__
#define __NAME_CLIENT2__

#include <yarp/YARPConfig.h>
#include <yarp/YARPAll.h>
#include <ace/config.h>

#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Log_Msg.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPString.h>

#define MAX_TELNET_BUFFER 512

class YARPNameClient2 {
public:
  YARPNameClient2();

  int isActive() {
    return active;
  }

  void activate(ACE_INET_Addr& nserver) {
    active = 1;
    server = nserver;
  }

  int registerName(const char *name, const char *ip, const char *type,
		   ACE_INET_Addr& addr);

  int queryName(const char *name, ACE_INET_Addr& addr, int *type);

  int check(const char *name, const char *key, const char *value);


private:
  int active;
  ACE_INET_Addr server;

  YARPString send(const YARPString& cmd, int multiline=0);

  char current[MAX_TELNET_BUFFER];
  int at, len;

  const char *receive(ACE_SOCK_Stream& is);
  int receive(ACE_SOCK_Stream& is, char *data, int len, double timeout=0);
};

#endif

