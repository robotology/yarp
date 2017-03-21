#include <yarp/os/BufferedPort.h>
//-----Openable and JoyPort are for confortable loop managing of ports
namespace yarp
{
    namespace dev
    {
        namespace JoypadControl
        {
            class                       Openable;
            template <typename T> class JoyPort;
        }
    }
}
struct yarp::dev::JoypadControl::Openable
{
    bool                  valid;
    int                   count;
    yarp::os::ConstString name;
    Openable():valid(false),count(0){}

    virtual ~Openable(){}
    virtual bool open()      = 0;
    virtual void interrupt() = 0;
    virtual void close()     = 0;
    virtual void write()     = 0;
};

template <typename T>
struct yarp::dev::JoypadControl::JoyPort : public yarp::dev::JoypadControl::Openable,
                                           public yarp::os::BufferedPort<T>
{
    typedef yarp::os::BufferedPort<T> bufferedPort;
    T               storage;
    yarp::os::Mutex mutex;
    T& prepare()                   {return bufferedPort::prepare();}
    bool open()      YARP_OVERRIDE {return bufferedPort::open(name);}
    T*   read()                    {return bufferedPort::read();}
    void interrupt() YARP_OVERRIDE {bufferedPort::interrupt();}
    void close()     YARP_OVERRIDE {bufferedPort::close();}
    void write()     YARP_OVERRIDE {bufferedPort::write();}

    using yarp::os::TypedReaderCallback<T>::onRead;
    virtual void onRead(T& datum) YARP_OVERRIDE
    {
        mutex.lock();
        storage = datum;
        mutex.unlock();
    }

};
//----------
