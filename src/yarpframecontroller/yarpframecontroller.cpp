#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/os/RFModule.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/FrameTransform.h>
#include "ParamParser.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::math;

constexpr size_t axisCount   = 6;
constexpr size_t buttonCount = 3;
constexpr double rate        = 0.01;

class FrameController : public RFModule
{
public:
    bool               useInternalTfServer;
    PolyDriver         joypadPD;
    PolyDriver         tfPD;
    IFrameTransform*   tfPublisher;
    IJoypadController* joypad;
    Matrix             left;
    Matrix             right;
    double             maxVelocity; //m/s
    double             velocity;   //velocity per cycle;
    double             axisVector[axisCount];
    float              buttonVector[buttonCount];
    FrameTransform     leftFrame;
    FrameTransform     rightFrame;
    double             L_homePos[3];
    double             R_homePos[3];
    ParamParser        param{"framecontroller"};
    

    FrameController() {}
    virtual double getPeriod() override { return rate; }
    
    virtual bool configure(ResourceFinder& rf) override
    {
        unsigned int count;
        Property     cfg;
        Bottle       b;
        param.parse(rf, "velocity",       ParamParser::TYPE_DOUBLE);
        param.parse(rf, "leftRootFrame",  ParamParser::TYPE_STRING);
        param.parse(rf, "rightRootFrame", ParamParser::TYPE_STRING);
        param.parse(rf, "leftFrame",      ParamParser::TYPE_STRING);
        param.parse(rf, "rightFrame",     ParamParser::TYPE_STRING);
        //param.parse(rf, "rightFrame",     ParamParser::TYPE_LIST);

        maxVelocity             = rf.find("velocity").asDouble();
        b                       = *rf.find("left_home_position").asList();
        velocity                = maxVelocity * rate;
        leftFrame.dst_frame_id  = rf.find("leftFrame").asString();
        leftFrame.src_frame_id  = rf.find("leftRootFrame").asString();
        rightFrame.dst_frame_id = rf.find("rightFrame").asString();
        rightFrame.src_frame_id = rf.find("rightRootFrame").asString();
        
        cfg.put("device", "JoypadControlClient");
        cfg.put("local", "/framecontroller");
        cfg.put("remote", rf.find("remote").asString());

        if (!joypadPD.open(cfg))
        {
            return false;
        }

        cfg.put("device", "transformClient");
        cfg.put("remote", "/transformServer");
        
        if (!tfPD.open(cfg))
        {
            return false;
        }

        if (!joypad->getAxisCount(count) || count < axisCount || !joypad->getButtonCount(count) || count < buttonCount)
        {
            yError() << "joypad not compliant.. you need" << axisCount <<  "axes and" << buttonCount << "button";
        }

        return true;
    }

    virtual bool   updateModule() override
    {
        size_t i;
        for (i = 0; i < buttonCount; ++i)
        {
            joypad->getButton(i, buttonVector[i]);
        }

        if (buttonVector[buttonCount - 1] > 0.5)
        {
            yInfo() << "home button pressed";
            leftFrame.translation.set( 0, 0, 0);
            rightFrame.translation.set(0, 0, 0);
        }

        for (i = 0; i < axisCount; ++i)
        {
            joypad->getAxis(i, axisVector[i]);
        }

        leftFrame.translation.set( leftFrame.translation.tX + axisVector[0]  * velocity, leftFrame.translation.tY + axisVector[1] * velocity, leftFrame.translation.tZ + axisVector[2]   * velocity);
        rightFrame.translation.set(rightFrame.translation.tX + axisVector[3] * velocity, rightFrame.translation.tY + axisVector[4] * velocity, rightFrame.translation.tZ + axisVector[5] * velocity);
        tfPublisher->setTransform(leftFrame.dst_frame_id, leftFrame.src_frame_id, leftFrame.toMatrix());
        tfPublisher->setTransform(rightFrame.dst_frame_id, rightFrame.src_frame_id, rightFrame.toMatrix());
        return true;
    }
};

int main(int argc, char *argv[])
{
    ResourceFinder& rf = ResourceFinder::getResourceFinderSingleton();
    rf.configure(argc, argv);

    FrameController fc;
    return fc.runModule(rf);
}

