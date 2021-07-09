/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>

#include "vocabs.h"
#include "client.h"

using namespace yarp;
using namespace yarp::os;
using namespace std;


CollatzClient::CollatzClient(const string &_portName)
{
    portName=_portName;
}


void CollatzClient::verifyItem(const unsigned int num, const unsigned int thres)
{
    unsigned int traj=num; // init the item trajectory
    unsigned int min=thres>1?thres:1;
    unsigned int cnt=0;

    fprintf(stdout,"Verifying item (num=%d, thres=%d): ... ",num,thres);

    // verify the Collatz conjecture for the specified item
    while (traj>min)
    {
        // bit-shifting is faster than division/multiplication
        if (traj&0x01)
            traj=(traj<<1)+traj+1;
        else
            traj>>=1;

        // prevent thread from taking
        // too many system resources
        if (++cnt>1e5)
        {
            Time::delay(0.01);
            cnt=0;
        }
    }

    // this point is reached iff the
    // conjecture is true, else the function
    // will keep on looping - in this case
    // it's up to the server to monitor an
    // unsatisfied request and decide what to do
    fprintf(stdout,"ok\n");
}


bool CollatzClient::threadInit()
{
    port.open(portName.c_str());
    replyField=COLLATZ_EMPTY_FIELD;

    return true;
}


void CollatzClient::run()
{
    Bottle in, out;

    while (!isStopping())
    {
        out.clear();
        out.addVocab32(COLLATZ_VOCAB_REQ_ITEM);
        out.addInt32(replyField);

        fprintf(stdout,"Requiring item: %s\n",out.toString().c_str());

        // forward a new request and wait for reply
        port.write(out,in);

        if (in.size()>0)
        {
            if (in.get(0).asVocab32()==COLLATZ_VOCAB_ITEM)
            {
                // process the reply
                const unsigned int num=(unsigned int)in.get(1).asInt32();
                const unsigned int thres=(unsigned int)in.get(2).asInt32();

                verifyItem(num,thres);
                replyField=num;
            }
        }
        else
            Time::delay(1.0);
    }
}


void CollatzClient::threadRelease()
{
    port.interrupt();
    port.close();
}
