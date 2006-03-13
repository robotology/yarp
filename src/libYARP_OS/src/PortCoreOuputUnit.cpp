

#include <yarp/os/Time.h>
#include <yarp/PortCoreOutputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/Name.h>
#include <yarp/Companion.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;

bool PortCoreOutputUnit::start() {

  phase.wait();

  bool result = PortCoreUnit::start();
  if (result) {
    phase.wait();
    phase.post();
  } else {
    phase.post();
  }

  return result;
}


void PortCoreOutputUnit::run() {
  running = true;
  phase.post();

  runSimulation();

  // it would be nice to get my entry removed from the port immediately,
  // but it would be a bit dodgy to delete this object and join this
  // thread within and from themselves
}



void PortCoreOutputUnit::runSimulation() {

  if (op!=NULL) {
    Route route = op->getRoute();
    if (Name(route.getToName()).isRooted()) {
      YARP_INFO(Logger::get(),String("Sending output from ") + 
		route.getFromName() + " to " + route.getToName() + " using " +
		route.getCarrierName());
    }
  }

  // no thread component at the moment
  running = false;
  return;

  /*
  // simulation
  running = true;
  while (true) {
    ACE_OS::printf("tick\n");
    Time::delay(0.3);
    if (closing||isDoomed()) {
      break;
    }
  }

  ACE_OS::printf("stopping\n");

  running = false;
  finished = true;
  */
}


void PortCoreOutputUnit::closeMain() {

  if (running) {
    // give a kick (unfortunately unavoidable)
    /*
    if (op!=NULL) {
      op->interrupt();
    }
    */
    closing = true;
    join();
  }

  if (op!=NULL) {
    Route route = op->getRoute();
    if (op->isConnectionless()) {
      YARP_DEBUG(Logger::get(),"asking other side to close, it is connectionless");
      try {
	Companion::disconnectInput(route.getToName().c_str(),
				   route.getFromName().c_str(),true);
      } catch (IOException e) {
	YARP_DEBUG(Logger::get(),e.toString() + 
		   " <<< exception during request to close input");
      }
    }
    if (Name(route.getToName()).isRooted()) {
      YARP_INFO(Logger::get(),String("Removing output from ") + 
		route.getFromName() + " to " + route.getToName());
    }
  }


  if (op!=NULL) {
    try {
      op->close();
    } catch (IOException e) { /*ok*/ }
    try {
      delete op;
    } catch (IOException e) { /*ok*/ }
    op = NULL;
  }
  running = false;
  closing = false;
  finished = true;
}


Route PortCoreOutputUnit::getRoute() {
  if (op!=NULL) {
    return op->getRoute();
  }
  return PortCoreUnit::getRoute();
}

void PortCoreOutputUnit::send(Writable& writer) {
  try {
    if (op!=NULL) {
      PortCommand pc('d',"");
      BufferedConnectionWriter buf(op->isTextMode());
      pc.writeBlock(buf);
      bool ok = writer.write(buf);
      if (!ok) {
	throw IOException("writer failed");
      }
      op->write(buf);
    }
  } catch (IOException e) {
    YARP_DEBUG(Logger::get(), e.toString() + " <<< output exception");
    finished = true;
    setDoomed(true);
  }
}




