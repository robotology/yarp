/*
 * Copyright (C) 2018 Istituto Italiano di Tecnologia (IIT)
 * Authors: Nicolò Genesio <nicolo.genesio@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/RFModule.h>
#include <yarp/os/PipelineThread.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/Network.h>

using namespace yarp::os;
using namespace yarp::os::impl;

class SummerThread : public PipelineThread<int, int>
{
public:
     SummerThread(AtomicBuffer<int> &bufferIn, AtomicBuffer<int> &bufferOut): PipelineThread(bufferIn, bufferOut)
     {}
protected:
     virtual void processData(int& dataIn, int& dataOut) override
     {
         int busy = 0;
         for(int i=0; i<1000; i++) // busy loop to check the multithreading
         {
             busy++;
         }
         busy--;
         //std::cout<<"Summer: dataIn "<<dataIn<<std::endl;
         dataOut = dataIn + 2;
         //std::cout<<"Summer: dataOut "<<dataOut<<std::endl;
     }

};

class DividerThread : public PipelineThread<int, double>
{
public:
    DividerThread(AtomicBuffer<int> &bufferIn, AtomicBuffer<double> &bufferOut): PipelineThread(bufferIn, bufferOut)
    {}
protected:
    virtual void processData(int& dataIn, double& dataOut) override
    {
        int busy = 0;
        for(int i=0; i<1000; i++) // busy loop to check the multithreading
        {
            busy++;
        }
        busy--;
        //std::cout<<"Divider: dataIn "<<dataIn<<std::endl;
        dataOut = dataIn/2.0;
        //std::cout<<"Divider: dataOut "<<dataOut<<std::endl;
    }


};


class PipelineModule : public RFModule
{
private:
    SummerThread* summerThread;
    DividerThread* dividerThread;

    AtomicBuffer<int> intBuffer1;
    AtomicBuffer<int> intBuffer2;
    AtomicBuffer<double> doubleBuffer;
    int iterationNum = 0;

public:
    PipelineModule()
    {}

    bool configure(yarp::os::ResourceFinder &rf)
    {
        summerThread = new SummerThread(intBuffer1, intBuffer2);
        dividerThread = new DividerThread(intBuffer2, doubleBuffer);

        bool ret = summerThread->start();

        return ret && dividerThread->start();
    }

    bool updateModule()
    {
        int data = iterationNum;

        intBuffer1.write(data);

        iterationNum ++;

        if (iterationNum >= 100)
        {
           return false ;
        }
        else
        {
            return true;
        }
    }

    bool getResult(double& result)
    {
        return doubleBuffer.read(result);
    }

    double getPeriod()
    {
        return 0.0;
    }

    bool interruptModule(){ return true; }

    bool close(){
        while (dividerThread->getCountProcessed() < 100)
                yarp::os::Time::delay(0.5);
        //stop threads
        summerThread->close();
        dividerThread->close();

        //deallocate memory
        delete summerThread;
        delete dividerThread;

        return true;
    }

};


class ConsProdThread :public PipelineThread<int, int>
{
public :
    ConsProdThread(AtomicBuffer<int> &bufferIn, AtomicBuffer<int> &bufferOut): PipelineThread(bufferIn, bufferOut)
    {}
protected:
     virtual void processData(int& dataIn, int& dataOut) override
    {
        dataOut = dataIn;
    }
};

class PipelineThreadTest : public UnitTest
{
public:
    virtual ConstString getName() override { return "PipelineThreadTest"; }

    void testModule()
    {
        report(0, "[Test] Checking PipelineModule...");

        PipelineModule mm;
        ResourceFinder rf;

        checkTrue(mm.configure(rf), "Checking configure....");
        mm.runModule();
    }

    void testSizeBuffer()
    {
        report(0,"[Test] Checking size and data consistency");
        AtomicBuffer<int> buffer, buffer2, buffer3;
        ConsProdThread producer(buffer, buffer2);
        ConsProdThread consumer(buffer2, buffer3);

        producer.start();
        for (int i =0; i<5; i++)
        {
            int data = i;
            buffer.write(data);
        }
        while (producer.getCountProcessed() != 5)
        {
            // waiting that all the data have been produced ...
            yarp::os::Time::delay(0.2);
        }
        bool checkSize = buffer2.getNumElements() == 5;

        checkSize &= buffer.getNumElements() == 0;
        checkTrue(checkSize, "checking the number of data produced");
        consumer.start();

        while (consumer.getCountProcessed() != 5)
        {
            // waiting that all the data have been consumed ...
            yarp::os::Time::delay(0.2);
        }

        checkSize = buffer2.getNumElements() == 0;
        checkSize &= buffer3.getNumElements() == 5;
        checkTrue(checkSize, "checking the number of data consumed");

        bool dataConsistent = true;
        for (int i = 0; i < 5; i++)
        {
            int data = 6;
            buffer3.read(data);
            dataConsistent &= data == i;
        }

        checkTrue(dataConsistent, "checking consistency of the data");

        producer.close();
        consumer.close();

    }

    virtual void runTests() override
    {
        Network::setLocalMode(true);

        testModule();
        testSizeBuffer();

        Network::setLocalMode(false);
    }
};


static PipelineThreadTest thePipelineThreadTest;

UnitTest& getPipelineThreadTest()
{
    return thePipelineThreadTest;
}


