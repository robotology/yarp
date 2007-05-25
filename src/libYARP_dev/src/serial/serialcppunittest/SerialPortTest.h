#ifndef SerialPortTest_H
#define SerialPortTest_H

#include <cppunit/extensions/HelperMacros.h>
#include <SerialHandler.h>
#include <ace/Task.h>
#include <DGSTask.h>

extern SerialHandler serialhandler;

class SerialPortTest : public CPPUNIT_NS::TestFixture, public DGSTask
{
  CPPUNIT_TEST_SUITE( SerialPortTest );
  CPPUNIT_TEST(testInitialize);
  CPPUNIT_TEST(testSerialComms);
  CPPUNIT_TEST_SUITE_END();

  void testSerialMessage(char * message, int message_size);

public:
  void setUp();
  void tearDown();

  void testInitialize();
  void testSerialComms();

  virtual int svc();

  virtual int IsAlive()
  { ACE_DEBUG((LM_INFO, ACE_TEXT("SerialPortTest: I am Alive!\n"))); return 0;};

  // SerialHandler  serialhandler;
};

#endif  // SerialPortTest_H
