#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "SerialPortTest.h"
#include <SerialHandler.h>

SerialHandler serialhandler;

class MyTestRunnner:  public DGSTask
{

public:

    virtual int svc()
    {
        ACE_OS::sleep(2);
        //CppUnit::TextUi::TestRunner runner;
        CppUnit::TextUi::TestRunner runner;
        runner.addTest(SerialPortTest::suite() );
        // Change the default outputter to a compiler error format outputter
        runner.setOutputter( new CPPUNIT_NS::TextOutputter( &runner.result(),
                CPPUNIT_NS::stdCOut() ) );
        runner.run();
        return 1;
    };

    virtual int IsAlive()
    { ACE_DEBUG((LM_INFO, ACE_TEXT("SerialPortTest: I am Alive!\n"))); return 0;};
};

int ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    //DGS_LOG_MANAGER->clearAllMessages();


    ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SILENT, ACE_TEXT("syslogTest"));
    //ACE_LOG_MSG->priority_mask (LM_TRACE | LM_DEBUG | LM_INFO | LM_NOTICE |
    //LM_ERROR | LM_WARNING, ACE_Log_Msg::THREAD);
    //ACE_LOG_MSG->open(argv[0],ACE_Log_Msg::STDERR, ACE_TEXT("syslogTest"));
    //ACE_LOG_MSG->priority_mask (LM_TRACE | LM_DEBUG | LM_INFO | LM_NOTICE | LM_ERROR | LM_WARNING, ACE_Log_Msg::THREAD);
    // dispatch ace events

    MyTestRunnner mytestrunner;
    mytestrunner.activate();

    int success = 1;
    success = !(ACE_Proactor::run_event_loop () == -1);
    return success ? 0 : 1;

    return 0;
}


