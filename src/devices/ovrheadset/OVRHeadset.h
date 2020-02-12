/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

#include <yarp/os/PeriodicThread.h>
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
#include <mutex>
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

/**
* @ingroup dev_impl_other
*
* \section OVRHeadset Description of input parameters
* \brief Device that manages the Oculus Rift Headset.
*
* Parameters accepted in the config argument of the open method:
* |   Parameter name      | Type   | Units | Default Value | Required  | Description                               | Notes |
* |:---------------------:|:------:|:-----:|:-------------:|:---------:|:-----------------------------------------:|:-----:|
* | tfLocal               | string |       |               | yes       | local port name receiving and posting tf  |       |
* | tfRemote              | string |       |               | yes       | name of the transformServer port          |       |
* | tf_left_hand_frame    | string |       |               | Yes       | name of the left hand frame               |       |
* | tf_right_hand_frame   | string |       |               | yes       | name of the right hand frame              |       |
* | tf_root_frame         | string |       |               | yes       | name of the root frame                    |       |
* | stick_as_axis         | bool   |       |               | yes       | if axes shoud be published as sticks      |       |
* | gui_elements          | int    |       |               | yes       | number of the gui element to visualize    |       |
* | hands_relative        | bool   |       | false         | no        | if the hand pose should be w.r.t. head    |       |

Gui Groups parameters
* |   Parameter name      | Type   | Units | Default Value | Required  | Description               | Notes |
* | width                 | double | pixel |               | yes       | width of the widget       |       |
* | height                | double | pixel |               | yes       | height of the widget      |       |
* | x                     | double | pixel |               | yes       | x position of the widget  |       |
* | y                     | double | pixel |               | yes       | y position of the widget  |       |
* | z                     | double | pixel |               | yes       | z position of the widget  |       |
* | alpha                 | double |       |               | yes       | alpha value of the widget |       |
**/
class OVRHeadset : public yarp::dev::DeviceDriver,
                   public yarp::os::PeriodicThread,
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
    bool getAxisCount(unsigned int& axis_count) override;
    bool getButtonCount(unsigned int& button_count) override;
    bool getTrackballCount(unsigned int& Trackball_count) override;
    bool getHatCount(unsigned int& Hat_count) override;
    bool getTouchSurfaceCount(unsigned int& touch_count) override;
    bool getStickCount(unsigned int& stick_count) override;
    bool getStickDoF(unsigned int stick_id, unsigned int& DoF) override;
    bool getButton(unsigned int button_id, float& value) override;
    bool getTrackball(unsigned int trackball_id, yarp::sig::Vector& value) override;
    bool getHat(unsigned int hat_id, unsigned char& value) override;
    bool getAxis(unsigned int axis_id, double& value) override;
    bool getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode) override;
    bool getTouch(unsigned int touch_id, yarp::sig::Vector& value) override;

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

    FlexImagePort* gui_ports{ nullptr };
    std::vector<guiParam> huds;
    InputCallback* displayPorts[2]{ nullptr, nullptr };
    ovrEyeRenderDesc EyeRenderDesc[2];
    TextureStatic* textureLogo{ nullptr };
    ovrLayerQuad logoLayer;
    TextureStatic* textureCrosshairs{ nullptr };
    ovrLayerQuad crosshairsLayer;
    TextureBattery* textureBattery{ nullptr };
    ovrLayerQuad batteryLayer;
    ovrMirrorTexture mirrorTexture{ nullptr };
    GLuint mirrorFBO{ 0 };
    ovrSession session;
    ovrHmdDesc hmdDesc;
    GLFWwindow* window{ nullptr };
    ovrTrackingState ts;
    ovrPoseStatef headpose;
    ovrPoseStatef predicted_headpose;
    unsigned int guiCount;
    bool         guiEnabled{ true };
    std::mutex                       inputStateMutex;
    ovrInputState                    inputState;
    bool                             inputStateError{ false };
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

    bool closed{ false };
    long long distortionFrameIndex{ 0 };

    unsigned int texWidth;
    unsigned int texHeight;
    double camHFOV[2];
    size_t camWidth[2];
    size_t camHeight[2];
    ovrFovPort fov[2];

    bool flipInputEnabled{ false };
    bool imagePoseEnabled{ true };
    bool userPoseEnabled{ false };

    // Layers
    bool logoEnabled{ true };
    bool crosshairsEnabled{ true };
    bool batteryEnabled{ true };

    double prediction;

}; // class OVRHeadset

} // namespace dev
} // namespace yarp


#endif // YARP_OVRHEADSET_OVRHEADSET_H
