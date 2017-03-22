#include <yarp/os/BufferedPort.h>
//-----Openable and JoyPort are for confortable loop managing of ports
namespace yarp
{
    namespace dev
    {
        namespace JoypadControl
        {
            class                       LoopablePort;
            template <typename T> class JoyPort;
        }
    }
}
struct yarp::dev::JoypadControl::LoopablePort
{
    bool                  valid;
    int                   count;
    yarp::os::ConstString name;
    LoopablePort():valid(false),count(0){}

    yarp::os::Contactable* contactable;
};

template <typename T>
struct yarp::dev::JoypadControl::JoyPort : public  yarp::dev::JoypadControl::LoopablePort,
                                           public  yarp::os::BufferedPort<T>
{
    typedef yarp::os::BufferedPort<T> bufferedPort;

    T               storage;
    yarp::os::Mutex mutex;

    JoyPort()                         {contactable = this;}

    using yarp::os::TypedReaderCallback<T>::onRead;
    virtual void onRead(T& datum) YARP_OVERRIDE
    {
        mutex.lock();
        storage = datum;
        mutex.unlock();
    }
};
//----------
