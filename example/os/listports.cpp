#include <yarp/os/all.h>
#include <yarp/os/impl/NameConfig.h>
#include <iostream>
using namespace yarp::os;
using namespace yarp::os::impl;

using namespace std;

int main(int argc, char *argv[]) {
    Network yarp;

    NameConfig nc;
    String name = nc.getNamespace();
    Bottle msg, reply;
    msg.addString("bot");
    msg.addString("list");

    Network::write(name.c_str(), msg, reply);

    for(int i=1; i<reply.size(); i++) {
      Property p;
      p.fromString(reply.get(i).toString());

      cout<<p.check("name", Value("[none]")).asString()<<"  "<<endl;
    }

    return 0;
}
