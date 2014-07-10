/** Copyright: (C) 2014 iCub Facility
* Authors: Lorenzo Natale
* CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include "SharedData.h"
#include <iostream>
#include <yarp/os/Network.h>
#include <yarp/os/Node.h>
#include <yarp/os/Publisher.h>
#include <yarp/os/Time.h>

using namespace std;

int main()
{
   yarp::os::Network network;

   yarp::os::Node node("/sender/node");   // added a Node
   yarp::os::Publisher<SharedData> port;  // changed Port to Publisher

   if (!port.topic("/data"))              // replaced open() with topic()
   {
       cerr<<"Error opening port, check your yarp network\n";
       return -1;
   }

   cout<<"Starting sender\n";
   double count=0.0;
   while(true)
   {
      SharedData d;

      // d.text is a string
      d.text="Hello from sender";
     
      //d.content is a vector, let's push some data
      d.content.push_back(count++);
      d.content.push_back(count++);

      port.write(d);

      yarp::os::Time::delay(0.1);
   }
  
   return 0;
}

