
#include <yarp/SocketTwoWayStream.h>

#include <ace/INET_Addr.h>

using namespace yarp;

int SocketTwoWayStream::open(const Address& address) {
  if (address.getPort()==-1) {
    return -1;
  }
  ACE_SOCK_Connector connector;
  ACE_INET_Addr addr(address.getPort(),address.getName().c_str());
  int result = connector.connect(stream,addr);
  if (result>=0) {
    happy = true;
  }
  updateAddresses();
  return result;
}

void SocketTwoWayStream::open(ACE_SOCK_Acceptor& acceptor) {
  int result = acceptor.accept(stream);
  if (result>=0) {
    happy = true;
  }
  updateAddresses();
}

void SocketTwoWayStream::updateAddresses() {
  ACE_INET_Addr local, remote;
  stream.get_local_addr(local);
  stream.get_remote_addr(remote);
  localAddress = Address(local.get_host_addr(),local.get_port_number());
  remoteAddress = Address(remote.get_host_addr(),remote.get_port_number());
}

