#ifndef YARP2_DATASOURCE
#define YARP2_DATASOURCE

#include <yarp/os/Port.h>
#include <yarp/os/Runnable.h>
#include <yarp/os/PortWriterBuffer.h>

namespace yarp {
    namespace dev {
        template <class T> class DataSource;
        template <class T> class DataWriter;
    }
}


template <class T>
class yarp::dev::DataSource {
public:
  virtual bool getDatum(T& datum) = 0;
};


template <class T>
class yarp::dev::DataWriter : public yarp::os::Runnable {
private:
  yarp::os::Port& port;
  yarp::os::PortWriterBuffer<T> writer;
  DataSource<T>& dater;
public:
  DataWriter(yarp::os::Port& port, DataSource<T>& dater) : 
    port(port), dater(dater)
  {
    writer.attach(port);
  }

  virtual void run() {
      T& datum = writer.get();
      dater.getDatum(datum);
      writer.write();
  }
};


#endif

