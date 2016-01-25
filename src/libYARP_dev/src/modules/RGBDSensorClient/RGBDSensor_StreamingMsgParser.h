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

typedef enum
{
    DEPTH_IMAGE,
    RGB_IMAGE,
    UNKNOWN
} RGBD_ImageType;

class YARP_dev_API IFlexImageReader
{
public:
    virtual bool updateImage(RGBD_ImageType, yarp::sig::FlexImage) = 0;
    virtual ~IFlexImageReader() {};
};

class YARP_dev_API FlexImageReader_Impl:  public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{
    RGBD_ImageType id;
    IFlexImageReader *p;

public:
    FlexImageReader_Impl();
    ~FlexImageReader_Impl();
    void configure(IFlexImageReader *_p, RGBD_ImageType i);
    void onRead(yarp::sig::FlexImage& datum);
    using yarp::os::TypedReaderCallback<yarp::sig::FlexImage>::onRead;
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
    RGBDSensor_StreamingMsgParser();
    bool updateImage(RGBD_ImageType id, yarp::sig::FlexImage data);
    bool synchRead(yarp::sig::FlexImage &data_1, yarp::sig::FlexImage &data_2);
    void attach(yarp::os::BufferedPort<yarp::sig::FlexImage> *port_1, yarp::os::BufferedPort<yarp::sig::FlexImage> *port_2);
};
