/*
 * Author: Lorenzo Natale
 * Copyright (C) 2015  Istituto Italiano di Tecnologia, iCub Facility
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/sig/Vector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

//#include <vector>

#include <yarp/gsl_compatibility.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;

template <class T>
class SenderThread:public Thread
{
public:
    SenderThread(Port *p)
    {
        portOut=p;
    }

    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
    {
        VectorOf<T> v;

        int times=10;

        while(times--)
        {
            v.clear();
            int k=0;
            for(k=0;k<10;k++)
                v.push_back(42);

            portOut->write(v);
            Time::delay(0.1);
            v.clear();

            for(k=0;k<42;k++)
                v.push_back(k);

            portOut->write(v);
        }

        success=true;
    }

    Port *portOut;
    bool success;
};

class Thread2b:public Thread
{
    int value;
public:
    Thread2b(Port *p)
    {
        portIn=p;
    }

    void init(int val)
    {
        value = val;
    }


    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
    {
        VectorOf<int> v;

        int times=10;
        bool ok=true;
        while(times--)
        {
            portIn->read(v);

            int k;
            int s=(int)v.size();
            if (s!=10)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=42)
                    ok=false;
            }

            portIn->read(v);
            s=(int)v.size();
            if (s!=42)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=k)
                    ok=false;
            }

        }

        success=ok;
    }

    Port *portIn;
    bool success;
};

class Thread3b :public Thread
{
public:
    Thread3b(Port *p)
    {
        portIn = p;
    }

    bool threadInit()
    {
        success = false;
        return true;
    }

    void run()
    {
        Bottle v;

        int times = 10;
        bool ok = true;
        while (times--)
        {
            portIn->read(v);

            int k;
            int s = (int)v.size();
            if (s != 10)
                ok = false;
            for (k = 0; k<s; k++)
            {
                if (v.get(k).asInt() != 42)
                {
                    
                    ok = false;
                }
            }

            portIn->read(v);

            s = (int)v.size();
            if (s != 42)
                ok = false;
            for (k = 0; k < s; k++)
            {
                if (v.get(k).asInt() != k)
                {
                    
                    ok = false;
                }
            }

        }

        success = ok;
    }

    Port *portIn;
    bool success;
};

class VectorOfTest : public UnitTest {
    
public:
    virtual String getName() { return "VectorOfTest"; }
    void checkSendReceiveInt()
    {
        report(0, "check VectorO<int> send receive");

        Port portIn;
        Port portOut;
                       
        portOut.open("/harness_sig/vtest/o");
        portIn.open("/harness_sig/vtest/i");

        Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

        portOut.enableBackgroundWrite(true);

      
        VectorOf<int> vector;
        vector.resize(10);
        for (unsigned int k = 0; k < vector.size(); k++)
        {
            vector[k] = k; 
        }
        
        portOut.write(vector);

        VectorOf<int> tmp;
        portIn.read(tmp);

        //compare vector and tmp
        bool success = true;
        if (tmp.size() != vector.size())
        {
            success = false;
        }
        else
        {
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                if (tmp[k] != vector[k])
                    success = false;
            }
        }

        checkTrue(success, "VectorOf<int> was sent and received correctly");
        
        report(0, "check VectorO<int> bottle compatibility");
        //write the same vector again and receive it as a bottle
        portOut.write(vector);
        Bottle tmp2;
        portIn.read(tmp2);

        //compare vector and tmp
        success = true;
        if (tmp2.size() != (int)vector.size())
        {
            success = false;
        }
        else
        {
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                if (tmp2.get(k).asInt() != vector[k])
                    success = false;
            }
        }

        checkTrue(success, "VectorOf<int> was received correctly in a Bottle");
    }

  
    virtual void runTests() {
        Network::setLocalMode(true);
        checkSendReceiveInt();
        Network::setLocalMode(false);
    }
};

static VectorOfTest theVectorOfTest;

UnitTest& getVectorOfTest() {
    return theVectorOfTest;
}
