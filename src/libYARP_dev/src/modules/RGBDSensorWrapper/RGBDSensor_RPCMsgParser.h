
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/DeviceDriver.h>

namespace sensor{
    namespace depth{
        class RGBDSensor_RPCMgsParser;
    }
}



class sensor::depth::RGBDSensor_RPCMgsParser: public yarp::dev::DeviceResponder
{
private:
    int verbose;
public:

    RGBDSensor_RPCMgsParser();
    ~RGBDSensor_RPCMgsParser();

  /**
    * Initialization.
    * @param x is the pointer to the instance of the object that uses the RPCMessagesParser.
    * This is required to recover the pointers to the interfaces that implement the responses
    * to the commands.
    */
//     void init(yarp::dev::ControlBoardWrapper *x);

    bool respond(const yarp::os::Bottle& command, yarp::os::Bottle& reply);

    // Helper functions
    // Verbose level goes from 0 to getMaxVerbose()
    bool setVerbose(int level);
    int getVerbose();
    int getMaxVerbose();
};
