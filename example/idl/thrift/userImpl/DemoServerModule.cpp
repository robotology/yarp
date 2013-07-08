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


class DemoServerModule : public yarp::test::Demo, public yarp::os::RFModule {
public:
    // Thrift Interface Implementation
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
  
  // RFModule implementation
  yarp::os::Port cmdPort;
  
  bool attach(yarp::os::Port &source)
  {
      return this->yarp().attachAsServer(source);
  }
  bool configure( yarp::os::ResourceFinder &rf )
    {
        std::string moduleName = rf.check("name", 
                yarp::os::Value("demoServerModule"), 
                "module name (string)").asString().c_str();
        setName(moduleName.c_str());
        
        std::string slash="/";
        
        attach(cmdPort);
        
        std::string cmdPortName= "/";
        cmdPortName+= getName();
        cmdPortName += "/cmd";
        if (!cmdPort.open(cmdPortName.c_str())) {           
            std::cout << getName() << ": Unable to open port " << cmdPortName << std::endl;  
            return false;
        }
        return true;
    }   
  bool updateModule()
  {
      return true;
  }
  bool close()
  {
      cmdPort.close(); 
      return true;
  }
};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cout<<"Error: yarp server does not seem available"<<std::endl;
        return -1;
    }
    
    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.configure("YARP_POLICY", argc, argv);

    DemoServerModule demoMod; 

    if (!demoMod.configure(rf))
        return -1;

    return demoMod.runModule();
}
