#include <string.h>

#include <yarp/os/PortablePair.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Log.h>

#include <yarp/sig/Vector.h>

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/PreciselyTimed.h>

#ifdef YARP_MSG
#include "jointData.h"
#endif

// encoders should arrive at least every 0.5s to be considered valide
// getEncoders will return false otherwise.
const double TIMEOUT_EXT=0.5;

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

class StateExtendedInputPort:public yarp::os::BufferedPort<jointData>
{
    jointData last;
    Semaphore mutex;
    Stamp lastStamp;
    double deltaT;
    double deltaTMax;
    double deltaTMin;
    double prev;
    double now;

    bool valid;
    int count;
public:

    StateExtendedInputPort();

    inline void resetStat();

    virtual void onRead(jointData &v);

    bool getLast(int j, jointData &data, Stamp &stamp, double &localArrivalTime);
    bool getLast(jointData &data2, Stamp &stmp, double &localArrivalTime);
    int  getIterations();

    // time is in ms
    void getEstFrequency(int &ite, double &av, double &min, double &max);
};

