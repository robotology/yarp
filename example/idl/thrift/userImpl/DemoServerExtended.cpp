/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <iostream>
#include <yarp/os/all.h>
#include <secondInterface/DemoExtended.h>


class DemoServerExtended : public yarp::test::DemoExtended
{
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
  
  virtual Point3D multiply_point(const Point3D& x, const double factor)
  {
      Point3D result;
      result.x=factor*x.x;
      result.y=factor*x.y;
      result.z=factor*x.z;
      return result;        
  }
    
};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;

    DemoServerExtended demoServer;
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
