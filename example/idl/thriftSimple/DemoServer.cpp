/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <iostream>
#include <yarp/os/all.h>

#include <Demo.h>

// ************************************* //
// We implement the Demo interface
// ************************************* //
class DemoServer : public Demo
{
  // Class members declaration
  int32_t answer;
  bool isRunning;
 
public:
  DemoServer();
  //function declarations, copied from Demo.h 
  virtual int32_t get_answer();
  virtual bool set_answer(int32_t rightAnswer);
  virtual int32_t add_one(const int32_t x);
  virtual bool start();
  virtual bool stop();
  virtual bool is_running();

//  virtual bool read(yarp::os::ConnectionReader& connection);
};

//implementation of each function:
DemoServer::DemoServer()
{
  // members initialization
  answer=42;
  isRunning=false;
  std::cout << "I know the answer!"<< std::endl; 
}

int32_t DemoServer::get_answer()
{
  std::cout << "The answer is "<< answer << std::endl;   
  return answer;
}

bool DemoServer::set_answer(int32_t rightAnswer)
{
  std::cout << "OMG are you serious? The answer is "<< rightAnswer << "?!?" << std::endl;
  answer=rightAnswer;
  return true;
}

int32_t DemoServer::add_one(const int32_t x)
{
  std::cout << "I'm adding one to "<< x << ". That's easy :) " << std::endl;
  return x+1;
}

bool DemoServer::start()
{
  std::cout << "Starting!" << std::endl;
  isRunning=true;
  return true;
}

bool DemoServer::stop()
{
  std::cout << "Stopping!" << std::endl; 
  isRunning=false;
  return true;
}

bool DemoServer::is_running()
{
  std::cout << "Indeed I am " << (isRunning ? "" : "not ") << "running" << std::endl;
  return isRunning;
}


// Instantiate the server and attach it to a YARP port
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
