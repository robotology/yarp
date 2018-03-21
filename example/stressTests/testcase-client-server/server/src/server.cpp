/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Bottle.h>
#include <stdio.h>

#include "vocabs.hpp"
#include "server.hpp"

using namespace yarp;
using namespace yarp::os;
using namespace std;


CollatzServer::CollatzServer(const string &_portName)
{
    portName=_portName;
}


void CollatzServer::tickItem(const unsigned int num)
{
    list<unsigned int>::iterator itr;

    // scan the table and remove the item verified by the client
    for (itr=table.begin(); itr!=table.end(); itr++)
        if (*itr==num)
        {
            fprintf(stdout,"Item %d verified\n",num);
            table.erase(itr);
            return;
        }

    fprintf(stdout,"Unknown item %d\n",num);
}


void CollatzServer::generateItem(unsigned int &num, unsigned int &thres)
{
    table.push_back(++curNum);

    num=curNum;

    // we know that front-1 has been already verified
    thres=table.front()-1;

    fprintf(stdout,"Generating item (num=%d, thres=%d)\n",num,thres);
}


bool CollatzServer::threadInit()
{
    port.open(portName.c_str());
    curNum=0;
    return true;
}


void CollatzServer::run()
{
    Bottle in, out;

    while (!isStopping())
    {        
        // wait for a client request
        if (port.read(in,true))
        {
     //       fprintf(stdout,"Request received\n");
#if 0
            if (in.get(0).asVocab()==COLLATZ_VOCAB_REQ_ITEM)
            {
                unsigned int num=in.get(1).asInt();

                fprintf(stdout,"Request received\n");

                // if the request contains a valid item,
                // then tick the item out of the table
                if (num!=COLLATZ_EMPTY_FIELD)
                    tickItem(num);
            }
#endif 
            // generate a new item
            unsigned int num=1;
            unsigned int thres=0;
            //generateItem(num,thres);

            out.clear();
            out.addVocab(COLLATZ_VOCAB_ITEM);
            out.addInt((int)num);
            out.addInt((int)thres);

            // send the item to the client
            port.reply(out);
        }
    }
}


void CollatzServer::threadRelease()
{
    table.clear();

    port.interrupt();
    port.close();
}



