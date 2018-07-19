/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef YARP_OVRHEADSET_OVRHEADSET_H
#define YARP_OVRHEADSET_OVRHEADSET_H

#include "ImageType.h"

#include <yarp/os/RateThread.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/IJoypadController.h>
#include <yarp/dev/IFrameTransform.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/Image.h>

#include <GL/glew.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <map>
#include <vector>



namespace yarp { namespace os { template <typename T> class BufferedPort; }}
namespace yarp { namespace os { class Bottle; }}
struct GLFWwindow;
class InputCallback;
class TextureStatic;
class TextureBattery;
struct guiParam;

namespace yarp {
namespace dev {

class OVRHeadset : public yarp::dev::DeviceDriver,
                   public yarp::os::SystemRateThread,
                   public yarp::dev::IService,
                   public yarp::dev::IJoypadController
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

    // yarp::dev::IJoypadController methods
    virtual bool getAxisCount(unsigned int& axis_count) override;
    virtual bool getButtonCount(unsigned int& button_count) override;
    virtual bool getTrackballCount(unsigned int& Trackball_count) override;
    virtual bool getHatCount(unsigned int& Hat_count) override;
    virtual bool getTouchSurfaceCount(unsigned int& touch_count) override;
    virtual bool getStickCount(unsigned int& stick_count) override;
    virtual bool getStickDoF(unsigned int stick_id, unsigned int& DoF) override;
    virtual bool getButton(unsigned int button_id, float& value) override;
    virtual bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) override;
    virtual bool getHat(unsigned int hat_id, unsigned char& value) override;
    virtual bool getAxis(unsigned int axis_id, double& value) override;
    virtual bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) override;
    virtual bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) override;

private:

    typedef yarp::os::BufferedPort<yarp::sig::FlexImage> FlexImagePort;

    bool createWindow(int w, int h);
    void onKey(int key, int scancode, int action, int mods);
    void reconfigureRendering();
    void reconfigureFOV();

    static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void glfwErrorCallback(int error, const char* description);
    static void ovrDebugCallback(uintptr_t userData, int level, const char* message);
    static void DebugHmd(ovrHmdDesc hmdDesc);
    void errorManager(ovrResult error);
    void fillAxisStorage();
    void fillErrorStorage();
    void fillButtonStorage();
    void fillHatStorage();
    void resetInput();


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
    FlexImagePort*                            gui_ports;
    
    std::vector<guiParam> huds;
    InputCallback* displayPorts[2];
    ovrEyeRenderDesc EyeRenderDesc[2];
    TextureStatic* textureLogo;
    ovrLayerQuad logoLayer;
    TextureStatic* textureCrosshairs;
    ovrLayerQuad crosshairsLayer;
    TextureBattery* textureBattery;
    ovrLayerQuad batteryLayer;
    ovrMirrorTexture mirrorTexture;
    GLuint mirrorFBO;
    ovrSession session;
    ovrHmdDesc hmdDesc;
    GLFWwindow* window;
    ovrTrackingState ts;
    ovrPoseStatef headpose;
    unsigned int guiCount;
    bool         enableGui;

    yarp::os::Mutex                  inputStateMutex;
    ovrInputState                    inputState;
    bool                             inputStateError;
    bool                             getStickAsAxis;
    std::vector<ovrButton>           buttonIdToOvrButton;
    std::vector<float*>              axisIdToValue;
    std::map<int, int>               DButtonToHat;
    std::map<ovrResult, std::string> error_messages;

    IFrameTransform* tfPublisher;
    bool             relative;
    std::string      left_frame;
    std::string      right_frame;
    std::string      root_frame;
    PolyDriver       driver;

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
