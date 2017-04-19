/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OVRHEADSET_OVRHEADSET_H
#define YARP_OVRHEADSET_OVRHEADSET_H

#include "ImageType.h"

#include <yarp/os/RateThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ServiceInterfaces.h>

#include <GL/glew.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>



namespace yarp { namespace os { template <typename T> class BufferedPort; }}
namespace yarp { namespace os { class Bottle; }}
struct GLFWwindow;
class InputCallback;
class TextureStatic;
class TextureBattery;

namespace yarp {
namespace dev {


class OVRHeadset : public yarp::dev::DeviceDriver,
                   public yarp::os::RateThread,
                   public yarp::dev::IService
{
public:
    explicit OVRHeadset();
    virtual ~OVRHeadset();

    // yarp::dev::DeviceDriver methods
    virtual bool open(yarp::os::Searchable& cfg);
    virtual bool close();

    // yarp::os::RateThread methods
    virtual bool threadInit();
    virtual void threadRelease();
    virtual void run();

    //  yarp::dev::IService methods
    virtual bool startService();
    virtual bool updateService();
    virtual bool stopService();


private:
    bool createWindow(int w, int h);
    void onKey(int key, int scancode, int action, int mods);
    void reconfigureRendering();
    void reconfigureFOV();

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwErrorCallback(int error, const char* description);
    static void ovrDebugCallback(uintptr_t userData, int level, const char* message);
    static void DebugHmd(ovrHmdDesc hmdDesc);

    yarp::os::BufferedPort<yarp::os::Bottle>* orientationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* positionPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* angularVelocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* linearVelocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* angularAccelerationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* linearAccelerationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedOrientationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedPositionPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedAngularVelocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedLinearVelocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedAngularAccelerationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* predictedLinearAccelerationPort;
    InputCallback* displayPorts[2];
    ovrEyeRenderDesc EyeRenderDesc[2];
    TextureStatic* textureLogo;
    TextureStatic* textureCrosshairs;
    TextureBattery* textureBattery;
    ovrMirrorTexture mirrorTexture;
    GLuint mirrorFBO;
    ovrSession session;
    ovrHmdDesc hmdDesc;
    GLFWwindow* window;

    bool closed;
    long long distortionFrameIndex;

    unsigned int texWidth;
    unsigned int texHeight;
    double camHFOV[2];
    unsigned int camWidth[2];
    unsigned int camHeight[2];
    ovrFovPort fov[2];

    bool flipInputEnabled;
    bool imagePoseEnabled;
    bool userPoseEnabled;

    // Layers
    bool logoEnabled;
    bool crosshairsEnabled;
    bool batteryEnabled;

    double prediction;

}; // class OVRHeadset

} // namespace dev
} // namespace yarp


#endif // YARP_OVRHEADSET_OVRHEADSET_H
