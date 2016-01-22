#include <list>
#include <yarp/sig/Image.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/os/PortablePair.h>
#include <yarp/os/LogStream.h>

typedef enum
{
    latest,
    sequenceNumber,
    timed
} SynchPolicy;

class YARP_dev_API IFlexImageReader
{
public:
    virtual bool updateImage(int, yarp::sig::FlexImage) = 0;
    virtual ~IFlexImageReader() {};
};

class FlexImageReader_Impl:  public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
    int id;
    IFlexImageReader *p;

public:

    FlexImageReader_Impl() { p = NULL;}
    void configure(IFlexImageReader *_p, int i)  { p = _p; id = i; };

    void onRead(yarp::sig::FlexImage& datum)
    {
        if(p != NULL)
            p->updateImage(id, datum);
        else
            yError() << "P is null";
    };
};


class RGBDSensor_StreamingMsgParser:    public IFlexImageReader
{
private:
    FlexImageReader_Impl read_1;
    FlexImageReader_Impl read_2;

    yarp::sig::FlexImage last_1;
    yarp::sig::FlexImage last_2;

    SynchPolicy policy;

public:
    RGBDSensor_StreamingMsgParser()
    {
        policy = latest;
        read_1.configure(this, 1);
        read_2.configure(this, 2);
    };

    bool updateImage(int id, yarp::sig::FlexImage data)
    {
        switch(id)
        {
            case 1:
                last_1 = data;
            break;

            case 2:
                last_2 = data;
            break;

            default:
                yError() << "Unknown id" << id;
            break;
        }
        return true;
    };

    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *port_1, yarp::os::BufferedPort<yarp::sig::FlexImage> *port_2)
    {
        port_1->useCallback(read_1);
        port_2->useCallback(read_2);
    }

    bool synchRead(yarp::sig::FlexImage *data_1, yarp::sig::FlexImage *data_2)
    {
        *data_1 = last_1;
        *data_2 = last_2;
        return true;
    }
};

