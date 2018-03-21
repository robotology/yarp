/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SharedData.h"
#include <iostream>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>

using namespace std;

int main()
{
   yarp::os::Network network;

   yarp::os::Port port;

   if (!port.open("/sender"))
   {
       cerr<<"Error opening port, check your yarp network\n";
       return -1;
   }
   cout<<"Starting sender\n";
   
   while(true)
   {
      SharedData d;

      // d.text is a string
      d.text="Hello from sender";

      //d.content is a vector, let's push some data
      d.content.push_back(0.0);
      d.content.push_back(0.0);

      port.write(d);

      yarp::os::Time::delay(0.1);
   }
  
   return 0;
}

