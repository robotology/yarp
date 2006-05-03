#include <yarp/os/Port.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/PortablePair.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

#include "TestList.h"

using namespace yarp::os;

using yarp::String;
using yarp::NetType;
using yarp::Logger;


class ServiceProvider : public PortReader {
public:

  virtual bool read(ConnectionReader& connection) {
    Bottle receive;
    //printf("service provider reading data\n");
    receive.read(connection);
    //printf("service provider read data\n");
    receive.addInt(5);
    ConnectionWriter *writer = connection.getWriter();
    if (writer!=NULL) {
      //printf("service provider replying\n");
      receive.write(*writer);
      //printf("service provider replied\n");
    }
    return true;
  }
};

class ServiceTester : public Portable {
public:
  yarp::UnitTest& owner;
  Bottle send, receive;
  int ct;

  ServiceTester(yarp::UnitTest& owner) : owner(owner) {}

  virtual bool write(ConnectionWriter& connection) {
    ct = 0;
    //printf("service tester writing data\n");
    send.write(connection);
    //printf("service tester wrote data\n");
    connection.setReplyHandler(*this);
    return true;
  }

  virtual bool read(ConnectionReader& connection) {
    //printf("service tester getting reply\n");
    receive.read(connection);
    //printf("service tester got reply\n");
    ct++;
    return true;
  }

  void finalCheck() {
    owner.checkEqual(receive.size(),send.size()+1,"size incremented");
    owner.checkEqual(ct,1,"just one read");
  }
};


class PortTest : public yarp::UnitTest {
public:
  virtual yarp::String getName() { return "PortTest"; }

  void testOpen() {
    report(0,"checking opening and closing ports");
    Port out, in;

    in.open("/in");
    out.open(Contact::bySocket("tcp","",9999));

    Contact conIn = in.where();
    Contact conOut = out.where();

    out.addOutput(Contact::byName("/in").addCarrier("udp"));
    Time::delay(0.2);

    checkEqual(conIn.getName().c_str(),"/in","name is recorded");

    checkTrue(String(conOut.getName().c_str()).strstr("/tmp")==0,
	      "name is created");

    Bottle bot1, bot2;
    bot1.fromString("5 10 \"hello\"");
    out.write(bot1);
    in.read(bot2);
    checkEqual(bot1.getInt(0),5,"check bot[0]");
    checkEqual(bot1.getInt(1),10,"check bot[1]");
    checkEqual(bot1.getString(2).c_str(),"hello","check bot[2]");

    bot1.fromString("18");
    out.write(bot1);
    in.read(bot2);
    checkEqual(bot1.getInt(0),18,"check one more send/receive");

    in.close();
    out.close();
  }


  void testBuffer() {
    report(0,"checking buffering");
    Bottle bot1;
    PortReaderBuffer<Bottle> buf;

    bot1.fromString("1 2 3");
    for (int i=0; i<10000; i++) {
      bot1.addInt(i);
    }

    Port input, output;
    input.open("/in");
    output.open("/out");

    input.setReader(buf);
    //buf.attach(input); // this is an alternative

    output.addOutput(Contact::byName("/in").addCarrier("tcp"));
    Time::delay(0.2);

    report(0,"writing...");
    output.write(bot1);
    output.write(bot1);
    output.write(bot1);
    report(0,"reading...");
    Bottle *result = buf.read();

    for (int j=0; j<3; j++) {
      if (j!=0) {
	result = buf.read();
      }
      checkTrue(result!=NULL,"got something check");
      if (result!=NULL) {
	checkEqual(bot1.size(),result->size(),"size check");
	YARP_INFO(Logger::get(),String("size is in fact ") + 
		  NetType::toString(result->size()));
      }
    }

    output.close();
    input.close();
  }

  void testUdp() {
    report(0,"checking udp");

    Bottle bot1;
    PortReaderBuffer<Bottle> buf;

    bot1.fromString("1 2 3");
    for (int i=0; i<10000; i++) {
      bot1.addInt(i);
    }

    Port input, output;
    input.open("/in");
    output.open("/out");

    input.setReader(buf);

    output.addOutput(Contact::byName("/in").addCarrier("udp"));
    Time::delay(0.2);

    report(0,"writing/reading three times...");

    report(0,"checking for whatever got through...");
    int ct = 0;
    while (buf.check()) {
      ct++;
      Bottle *result = buf.read();
      checkTrue(result!=NULL,"got something check");
      if (result!=NULL) {
	checkEqual(bot1.size(),result->size(),"size check");
	YARP_INFO(Logger::get(),String("size is in fact ") + 
		  NetType::toString(result->size()));
      }
    }
    if (ct==0) {
      report(0,"NOTHING got through - possible but sad");
    }

    output.close();
    input.close();
  }


  void testHeavy() {
    report(0,"checking heavy udp");

    Bottle bot1;
    PortReaderBuffer<Bottle> buf;

    bot1.fromString("1 2 3");
    for (int i=0; i<100000; i++) {
      bot1.addInt(i);
    }

    Port input, output;
    input.open("/in");
    output.open("/out");

    input.setReader(buf);

    output.addOutput(Contact::byName("/in").addCarrier("udp"));
    Time::delay(0.2);


    for (int j=0; j<3; j++) {
      report(0,"writing/reading three times...");
      output.write(bot1);
    }

    report(0,"checking for whatever got through...");
    int ct = 0;
    while (buf.check()) {
      ct++;
      Bottle *result = buf.read();
      checkTrue(result!=NULL,"got something check");
      if (result!=NULL) {
	checkEqual(bot1.size(),result->size(),"size check");
	YARP_INFO(Logger::get(),String("size is in fact ") + 
		  NetType::toString(result->size()));
      }
    }
    if (ct==0) {
      report(0,"NOTHING got through - possible but sad");
    }

    output.close();
    input.close();
  }


  void testPair() {
    report(0,"checking paired send/receive");
    PortReaderBuffer<PortablePair<Bottle,Bottle> > buf;

    Port input, output;
    input.open("/in");
    output.open("/out");

    input.setReader(buf);

    output.addOutput(Contact::byName("/in").addCarrier("tcp"));
    Time::delay(0.2);


    PortablePair<Bottle,Bottle> bot1;
    bot1.head.fromString("1 2 3");
    bot1.body.fromString("4 5 6 7");

    report(0,"writing...");
    output.write(bot1);
    report(0,"reading...");
    PortablePair<Bottle,Bottle> *result = buf.read();

    checkTrue(result!=NULL,"got something check");
    checkEqual(bot1.head.size(),result->head.size(),"head size check");
    checkEqual(bot1.body.size(),result->body.size(),"body size check");

    output.close();
    input.close();
  }


  void testReply() {
    report(0,"checking reply processing");
    ServiceProvider provider;

    Port input, output;
    input.open("/in");
    output.open("/out");
    
    input.setReader(provider);
    
    output.addOutput(Contact::byName("/in").addCarrier("tcp"));
    Time::delay(0.1);
    ServiceTester tester(*this);
    output.write(tester);
    Time::delay(0.1);
    tester.finalCheck();
    Time::delay(0.1);
    output.close();
    input.close();    
  }

  virtual void runTests() {
    yarp::NameClient& nic = yarp::NameClient::getNameClient();
    nic.setFakeMode(true);
    testOpen();
    testBuffer();
    testPair();
    testReply();
    //    testUdp();
    //    testHeavy();

    nic.setFakeMode(false);
  }
};

static PortTest thePortTest;

yarp::UnitTest& getPortTest() {
  return thePortTest;
}

