
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Os.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Node.h>
#include "include/geometry_msgs_TransformStamped.h"
#include "include/tf_tfMessage.h"
#include <yarp/os/Publisher.h>
#include <yarp/math/Math.h>
#include <limits>
#include <iostream>
#include <iomanip>
#include <string>

#define X_AX 0
#define Y_AX 1
#define Z_AX 2
#define W_AX 3

#define ROLL  0
#define PITCH 1
#define YAW   2

struct tf
{ 
    enum            tf_type_enum { fixed = 0, external = 1 } type;
    double          tX;
    double          tY;
    double          tZ;
    double          rX;
    double          rY;
    double          rZ;
    double          rW; 
    double          timeStamp;
    double          lifeTime;
    std::string     name;
    std::string     parent_frame;
    std::string     child_frame;

    //yarp::os::BufferedPort<yarp::os::Bottle>* tfport;

                       tf();
                       tf
                       (
                            const std::string& inName,
                            const std::string& parent,
                            const std::string& child,
                            double             inTX,
                            double             inTY,
                            double             inTZ,
                            double             inRX,
                            double             inRY,
                            double             inRZ,
                            double             inRW
                       );
                       ~tf();
    //void   read();
    void               transFromVec(double X, double Y, double Z);
    void               rotFromRPY(double R, double P, double Y);
    yarp::sig::Vector  getRPYRot() const;
};