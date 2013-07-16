/* 
 * Copyright (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
 * Author: Elena Ceseracciu
 * email:  elena.ceseracciu@iit.it
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
*/

#include <iostream>
#include <yarp/os/all.h>
#include <secondInterface/Demo.h>


class DemoServer : public yarp::test::Demo {
public:
  virtual int32_t get_answer()
  {
      std::cout<<"Server:get_answer called" <<std::endl;
      return ANSWER;
  }
  virtual int32_t add_one(const int32_t x = 0)
  {
     std::cout<<"Server::add_one called with "<< x <<std::endl;
      return x+1;
  }
  virtual int32_t double_down(const int32_t x)
  {
      std::cout<<"Server::double_down called with "<< x <<std::endl;
      return x*2;
  }
  virtual  ::yarp::test::PointD add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y)
  {
    std::cout<<"Server::add_point called"<<std::endl;
    ::yarp::test::PointD z;
    z.x = x.x + y.x;
    z.y = x.y + y.y;
    z.z = x.z + y.z;
    return z;
  }
};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;

    DemoServer demoServer;
    yarp::os::Port port;
    demoServer.yarp().attachAsServer(port);
    if (!port.open("/demoServer")) { return 1; }
    
    while (true) {
        printf("Server running happily\n");
        yarp::os::Time::delay(10);
    }
    port.close();
    return 0;
}
