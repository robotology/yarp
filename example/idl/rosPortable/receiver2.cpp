/** Copyright: (C) 2014 iCub Facility
* Authors: Lorenzo Natale
* CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <SharedData.h>
#include <iostream>
#include <yarp/os/Network.h>
#include <yarp/os/Node.h>
#include <yarp/os/Subscriber.h>

using namespace std;

int main()
{
   yarp::os::Network network;

   cout<<"Starting receiver\n";

   yarp::os::Node node("/receiver/node");  // added a Node
   yarp::os::Subscriber<SharedData> port;  // changed Port to Subscriber
   if (!port.topic("/data"))               // replaced open() with topic()
   {
       cerr<<"Error opening port, check your yarp network\n";
       return -1;
   }

   // network.connect("/sender", "/receiver"); // don't need this anymore

   int count=0;
   while(true)
   {
       SharedData d;
       port.read(d);

       //access d
       cout << count << " Received SharedData:\n";
       cout << d.text << "\n";
       for (int i=0; i<d.content.size(); i++)
       {
          cout<<d.content[i]<<" ";
       }
       cout<<"\n";
       count++;
   }
   
   return 0;
}

