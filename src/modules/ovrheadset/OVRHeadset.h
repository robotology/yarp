/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
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
    bool createWindow(int w, int h, int x = 0, int y = 0);
    void onKey(int key, int scancode, int action, int mods);
    void reconfigureRendering();
    void reconfigureFOV();

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwErrorCallback(int error, const char* description);
    static void checkGlError(const char* file, int line);
    static void ovrDebugCallback(int level, const char* message);
    static void DebugHmd(ovrHmd hmd);

    yarp::os::BufferedPort<yarp::os::Bottle>* orientationPort;
    yarp::os::BufferedPort<yarp::os::Bottle>* positionPort;
#if !XXX_DUAL
    yarp::os::BufferedPort<ImageType>* displayPorts[2];
    InputCallback* displayPortCallbacks[2];
#else
    yarp::os::BufferedPort<ImageType>* displayPort;
    InputCallback* displayPortCallback;
#endif
    ovrEyeRenderDesc EyeRenderDesc[2];

    ovrHmd hmd;
    GLFWwindow* window;

    ovrGLConfig config;


    bool closed;
    unsigned int distortionFrameIndex;

    unsigned int texWidth;
    unsigned int texHeight;
    double camHFOV[2];
    unsigned int camWidth[2];
    unsigned int camHeight[2];
    ovrFovPort fov[2];

    bool multiSampleEnabled;
    bool overdriveEnabled;
    bool hqDistortionEnabled;
    bool flipInputEnabled;
    bool timeWarpEnabled;
    bool imagePoseEnabled;

}; // class OVRHeadset

} // namespace dev
} // namespace yarp


#endif // YARP_OVRHEADSET_OVRHEADSET_H
