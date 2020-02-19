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

#define _USE_MATH_DEFINES

#include "OVRHeadset.h"
#include "InputCallback.h"
#include "TextureBuffer.h"
#include "TextureStatic.h"
#include "TextureBattery.h"
#include "GLDebug.h"

#include "img-yarp-robot-64.h"
#include "img-crosshairs.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>
#include <yarp/os/Property.h>
#include <yarp/os/SystemClock.h>
#include <yarp/math/FrameTransform.h>

#include <cmath>
#include <mutex>
#include <unordered_map>
#include <OVR_CAPI_Util.h>
#include <OVR_Math.h>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif
YARP_CONSTEXPR unsigned int AXIS_COUNT   = 8;
YARP_CONSTEXPR unsigned int STICK_COUNT  = 2;
YARP_CONSTEXPR unsigned int BUTTON_COUNT = 13;

#if defined(_WIN32)
 #define GLFW_EXPOSE_NATIVE_WIN32
 #define GLFW_EXPOSE_NATIVE_WGL
 #define OVR_OS_WIN32
#elif defined(__APPLE__)
 #define GLFW_EXPOSE_NATIVE_COCOA
 #define GLFW_EXPOSE_NATIVE_NSGL
 #define OVR_OS_MAC
#elif defined(__linux__)
 #define GLFW_EXPOSE_NATIVE_X11
 #define GLFW_EXPOSE_NATIVE_GLX
 #define OVR_OS_LINUX
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <OVR_System.h>
#include <OVR_CAPI_GL.h>


#ifdef check
// Undefine the check macro in AssertMacros.h on OSX or the "cfg.check"
// call will fail compiling.
#undef check
#endif

typedef bool(yarp::os::Value::*valueIsType)(void) const;
typedef yarp::os::BufferedPort<yarp::sig::FlexImage> FlexImagePort;
struct guiParam
{
    double         resizeW;
    double         resizeH;
    double         x;
    double         y;
    double         z;
    double         alpha;
    FlexImagePort* port;
    ovrLayerQuad   layer;
    TextureBuffer*  texture;
};
//----------------[utilities]
//WARNING it makes a conversion of the coordinate system
inline yarp::sig::Vector ovrVec3ToYarp(const ovrVector3f& v)
{
    yarp::sig::Vector ret(3);

    ret[0] = -v.z;
    ret[1] = -v.x;
    ret[2] =  v.y;

    return ret;
}

//WARNING it makes a conversion of the coordinate system
inline yarp::sig::Vector ovrRot2YarpRPY(const OVR::Quatf& rot)
{
    float yaw, pitch, roll;
    yarp::sig::Vector v(3);

    rot.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
    v[0] = -roll; v[1] = -pitch; v[2] = yaw;

    return v;
}

inline yarp::sig::Matrix ovr2matrix(const ovrVector3f& pos, const OVR::Quatf& orientation)
{
    yarp::sig::Matrix ret;
    ret  = yarp::math::rpy2dcm(ovrRot2YarpRPY(orientation));
    ret.setSubcol(ovrVec3ToYarp(pos), 0, 3);

    return ret;
}

inline ovrVector3f vecSubtract(const ovrVector3f& a, const ovrVector3f& b)
{
    ovrVector3f ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}

static inline void debugTangent(std::string message, float tangent1, float tangent2)
{
    yDebug((message + "    %10f (%5f[rad] = %5f[deg])        %10f (%5f[rad] = %5f[deg])\n").c_str(),
        tangent1,
        atan(tangent1),
        OVR::RadToDegree(atan(tangent1)),
        tangent2,
        atan(tangent2),
        OVR::RadToDegree(atan(tangent2)));
}
static void debugFov(const ovrFovPort fov[2]) {
    yDebug("             Left Eye                                           Right Eye\n");
    debugTangent("LeftTan",  fov[0].LeftTan , fov[0].LeftTan);
    debugTangent("RightTan", fov[0].RightTan, fov[0].RightTan);
    debugTangent("UpTan",    fov[0].UpTan   , fov[0].UpTan   );
    debugTangent("DownTan",  fov[0].DownTan , fov[0].DownTan );
    yDebug("\n\n\n");
}

static int compareLuid(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
    return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}

static ovrGraphicsLuid GetDefaultAdapterLuid()
{
    ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
    IDXGIFactory* factory = nullptr;

    if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
    {
        IDXGIAdapter* adapter = nullptr;

        if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
        {
            DXGI_ADAPTER_DESC desc;

            adapter->GetDesc(&desc);
            memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
            adapter->Release();
        }

        factory->Release();
    }
#endif

    return luid;
}

inline void writeVec3OnPort(yarp::os::BufferedPort<yarp::os::Bottle>*const & port, const OVR::Vector3f& vec3, yarp::os::Stamp& stamp)
{
    if (port || port->getOutputCount() > 0)
    {
        yarp::os::Bottle& output = port->prepare();
        output.clear();
        output.addFloat64(vec3.x);
        output.addFloat64(vec3.y);
        output.addFloat64(vec3.z);
        port->setEnvelope(stamp);
        port->write();
    }
}

inline OVR::Vector3f radToDeg(const OVR::Vector3f& v)
{
    OVR::Vector3f ret;

    ret.x = OVR::RadToDegree(v.x);
    ret.y = OVR::RadToDegree(v.y);
    ret.z = OVR::RadToDegree(v.z);

    return ret;
}

inline void setHeadLockedLayer(ovrLayerQuad& layer, TextureStatic* tex,
                               const float x,     const float y,  const float z, //position
                               const float rx,    const float ry, const float rz, float rw, //rotation
                               const float sizeX, const float sizeY)//scale
{
    layer.Header.Type                  = ovrLayerType_Quad;
    layer.Header.Flags                 = ovrLayerFlag_HeadLocked;
    layer.ColorTexture                 = tex->textureSwapChain;
    layer.QuadPoseCenter.Position.x    = x;
    layer.QuadPoseCenter.Position.y    = y;
    layer.QuadPoseCenter.Position.z    = z;
    layer.QuadPoseCenter.Orientation.x = rx;
    layer.QuadPoseCenter.Orientation.y = ry;
    layer.QuadPoseCenter.Orientation.z = rz;
    layer.QuadPoseCenter.Orientation.w = rw;
    layer.QuadSize.x                   = sizeX;
    layer.QuadSize.y                   = sizeY;

    layer.Viewport = OVR::Recti(0, 0, tex->width, tex->height);
}

inline void setHeadLockedLayer(ovrLayerQuad& layer, TextureBuffer* tex,
    const float x, const float y, const float z, //position
    const float rx, const float ry, const float rz, float rw, //rotation
    const float sizeX, const float sizeY)//scale
{
    layer.Header.Type = ovrLayerType_Quad;
    layer.Header.Flags = ovrLayerFlag_HeadLocked;
    layer.ColorTexture = tex->textureSwapChain;
    layer.QuadPoseCenter.Position.x = x;
    layer.QuadPoseCenter.Position.y = y;
    layer.QuadPoseCenter.Position.z = z;
    layer.QuadPoseCenter.Orientation.x = rx;
    layer.QuadPoseCenter.Orientation.y = ry;
    layer.QuadPoseCenter.Orientation.z = rz;
    layer.QuadPoseCenter.Orientation.w = rw;
    layer.QuadSize.x = sizeX;
    layer.QuadSize.y = sizeY;

    layer.Viewport = OVR::Recti(0, 0, tex->width, tex->height);
}

//----------------end [utilities]

yarp::dev::OVRHeadset::OVRHeadset() :
        yarp::dev::DeviceDriver(),
        yarp::os::PeriodicThread(0.011, yarp::os::ShouldUseSystemClock::Yes) // ~90 fps
{
    yTrace();
}
yarp::dev::OVRHeadset::~OVRHeadset()
{
    yTrace();
}

void yarp::dev::OVRHeadset::fillAxisStorage()
{
    axisIdToValue.push_back(inputState.IndexTrigger);
    axisIdToValue.push_back(inputState.IndexTrigger + 1);
    axisIdToValue.push_back(inputState.HandTrigger);
    axisIdToValue.push_back(inputState.HandTrigger + 1 );

    if (getStickAsAxis)
    {
        axisIdToValue.push_back(&inputState.Thumbstick[ovrHand_Left].x);
        axisIdToValue.push_back(&inputState.Thumbstick[ovrHand_Left].y);
        axisIdToValue.push_back(&inputState.Thumbstick[ovrHand_Right].x);
        axisIdToValue.push_back(&inputState.Thumbstick[ovrHand_Right].y);
    }

}

void yarp::dev::OVRHeadset::fillErrorStorage()
{
    error_messages[ovrError_MemoryAllocationFailure       ] = "Failure to allocate memory.";
    error_messages[ovrError_InvalidSession                ] = "Invalid ovrSession parameter provided.";
    error_messages[ovrError_Timeout                       ] = "The operation timed out.";
    error_messages[ovrError_NotInitialized                ] = "The system or component has not been initialized.";
    error_messages[ovrError_InvalidParameter              ] = "Invalid parameter provided.See error info or log for details.";
    error_messages[ovrError_ServiceError                  ] = "Generic service error.See error info or log for details.";
    error_messages[ovrError_NoHmd                         ] = "The given HMD doesn't exist.";
    error_messages[ovrError_Unsupported                   ] = "Function call is not supported on this hardware / software.";
    error_messages[ovrError_DeviceUnavailable             ] = "Specified device type isn't available.";
    error_messages[ovrError_InvalidHeadsetOrientation     ] = "The headset was in an invalid orientation for the requested operation(e.g.vertically oriented during ovr_RecenterPose).";
    error_messages[ovrError_ClientSkippedDestroy          ] = "The client failed to call ovr_Destroy on an active session before calling ovr_Shutdown.Or the client crashed.";
    error_messages[ovrError_ClientSkippedShutdown         ] = "The client failed to call ovr_Shutdown or the client crashed.";
    error_messages[ovrError_ServiceDeadlockDetected       ] = "The service watchdog discovered a deadlock.";
    error_messages[ovrError_InvalidOperation              ] = "Function call is invalid for object's current state.";
    error_messages[ovrError_AudioDeviceNotFound           ] = "Failure to find the specified audio device.";
    error_messages[ovrError_AudioComError                 ] = "Generic COM error.";
    error_messages[ovrError_Initialize                    ] = "Generic initialization error.";
    error_messages[ovrError_LibLoad                       ] = "Couldn't load LibOVRRT.";
    error_messages[ovrError_LibVersion                    ] = "LibOVRRT version incompatibility.";
    error_messages[ovrError_ServiceConnection             ] = "Couldn't connect to the OVR Service.";
    error_messages[ovrError_ServiceVersion                ] = "OVR Service version incompatibility.";
    error_messages[ovrError_IncompatibleOS                ] = "The operating system version is incompatible.";
    error_messages[ovrError_DisplayInit                   ] = "Unable to initialize the HMD display.";
    error_messages[ovrError_ServerStart                   ] = "Unable to start the server.Is it already running ?";
    error_messages[ovrError_Reinitialization              ] = "Attempting to re - initialize with a different version.";
    error_messages[ovrError_MismatchedAdapters            ] = "Chosen rendering adapters between client and service do not match.";
    error_messages[ovrError_LeakingResources              ] = "Calling application has leaked resources.";
    error_messages[ovrError_ClientVersion                 ] = "Client version too old to connect to service.";
    error_messages[ovrError_OutOfDateOS                   ] = "The operating system is out of date.";
    error_messages[ovrError_OutOfDateGfxDriver            ] = "The graphics driver is out of date.";
    error_messages[ovrError_IncompatibleGPU               ] = "The graphics hardware is not supported.";
    error_messages[ovrError_NoValidVRDisplaySystem        ] = "No valid VR display system found.";
    error_messages[ovrError_Obsolete                      ] = "Feature or API is obsolete and no longer supported.";
    error_messages[ovrError_DisabledOrDefaultAdapter      ] = "No supported VR display system found, but disabled or driverless adapter found.";
    error_messages[ovrError_HybridGraphicsNotSupported    ] = "The system is using hybrid graphics(Optimus, etc...), which is not support.";
    error_messages[ovrError_DisplayManagerInit            ] = "Initialization of the DisplayManager failed.";
    error_messages[ovrError_TrackerDriverInit             ] = "Failed to get the interface for an attached tracker.";
    error_messages[ovrError_LibSignCheck                  ] = "LibOVRRT signature check failure.";
    error_messages[ovrError_LibPath                       ] = "LibOVRRT path failure.";
    error_messages[ovrError_LibSymbols                    ] = "LibOVRRT symbol resolution failure.";
    error_messages[ovrError_RemoteSession                 ] = "Failed to connect to the service because remote connections to the service are not allowed.";
    error_messages[ovrError_DisplayLost                   ] = "In the event of a system - wide graphics reset or cable unplug this is returned to the app.";
    error_messages[ovrError_TextureSwapChainFull          ] = "ovr_CommitTextureSwapChain was called too many times on a texture swapchain without calling submit to use the chain.";
    error_messages[ovrError_TextureSwapChainInvalid       ] = "The ovrTextureSwapChain is in an incomplete or inconsistent state.Ensure ovr_CommitTextureSwapChain was called at least once first.";
    error_messages[ovrError_GraphicsDeviceReset           ] = "Graphics device has been reset(TDR, etc...)";
    error_messages[ovrError_DisplayRemoved                ] = "HMD removed from the display adapter.";
    error_messages[ovrError_ContentProtectionNotAvailable ] = "Content protection is not available for the display.";
    error_messages[ovrError_ApplicationInvisible          ] = "Application declared itself as an invisible type and is not allowed to submit frames.";
    error_messages[ovrError_Disallowed                    ] = "The given request is disallowed under the current conditions.";
    error_messages[ovrError_DisplayPluggedIncorrectly     ] = "Display portion of HMD is plugged into an incompatible port(ex: IGP)";
    error_messages[ovrError_RuntimeException              ] = "A runtime exception occurred.The application is required to shutdown LibOVR and re - initialize it before this error state will be cleared.";
    error_messages[ovrError_NoCalibration                 ] = "Result of a missing calibration block.";
    error_messages[ovrError_OldVersion                    ] = "Result of an old calibration block.";
    error_messages[ovrError_MisformattedBlock             ] = "Result of a bad calibration block due to lengths.";
}

void yarp::dev::OVRHeadset::fillButtonStorage()
{
    buttonIdToOvrButton.push_back(ovrButton_A);
    buttonIdToOvrButton.push_back(ovrButton_B);
    buttonIdToOvrButton.push_back(ovrButton_RThumb);
    buttonIdToOvrButton.push_back(ovrButton_RShoulder);
    buttonIdToOvrButton.push_back(ovrButton_X);
    buttonIdToOvrButton.push_back(ovrButton_Y);
    buttonIdToOvrButton.push_back(ovrButton_LThumb);
    buttonIdToOvrButton.push_back(ovrButton_LShoulder);
    buttonIdToOvrButton.push_back(ovrButton_Enter);
    buttonIdToOvrButton.push_back(ovrButton_Back);
    buttonIdToOvrButton.push_back(ovrButton_VolUp);
    buttonIdToOvrButton.push_back(ovrButton_VolDown);
    buttonIdToOvrButton.push_back(ovrButton_Home);
}

void yarp::dev::OVRHeadset::fillHatStorage()
{
    DButtonToHat[0]               = YRPJOY_HAT_CENTERED;
    DButtonToHat[ovrButton_Up]    = YRPJOY_HAT_UP;
    DButtonToHat[ovrButton_Right] = YRPJOY_HAT_RIGHT;
    DButtonToHat[ovrButton_Down]  = YRPJOY_HAT_DOWN;
    DButtonToHat[ovrButton_Left]  = YRPJOY_HAT_LEFT;
}

bool yarp::dev::OVRHeadset::open(yarp::os::Searchable& cfg)
{
    yTrace();

    typedef std::vector<std::pair<yarp::os::BufferedPort<yarp::os::Bottle>**, std::string> > port_params;
    typedef std::vector<std::tuple<std::string, std::string, bool*, bool> >                 optionalParamType;

    yarp::os::Property tfClientCfg;
    port_params        ports;
    optionalParamType  optionalParams;
    std::string        standardPortPrefix;

    standardPortPrefix = "/oculus";

    //checking all the parameter in the configuration file..
    {
        constexpr unsigned int STRING = 0;
        constexpr unsigned int BOOL   = 1;
        constexpr unsigned int INT    = 2;
        constexpr unsigned int DOUBLE = 3;

        std::map<int, std::string>                err_msgs;
        std::map<int, valueIsType>                isFunctionMap;
        std::vector<std::pair<std::string, int> > paramParser;

        err_msgs[STRING]      = "a string";
        err_msgs[BOOL]        = "a boolean type";
        err_msgs[INT]         = "an integer type";
        err_msgs[DOUBLE]      = "a real type";
        isFunctionMap[STRING] = &yarp::os::Value::isString;
        isFunctionMap[BOOL]   = &yarp::os::Value::isBool;
        isFunctionMap[INT]    = &yarp::os::Value::isInt32;
        isFunctionMap[DOUBLE] = &yarp::os::Value::isFloat64;

        //to add a parameter check, simply add a line below here and let the magic happens
        paramParser.push_back(std::make_pair("tfDevice",            STRING));
        paramParser.push_back(std::make_pair("tfLocal",             STRING));
        paramParser.push_back(std::make_pair("tfRemote",            STRING));
        paramParser.push_back(std::make_pair("tf_left_hand_frame",  STRING));
        paramParser.push_back(std::make_pair("tf_right_hand_frame", STRING));
        paramParser.push_back(std::make_pair("tf_root_frame",       STRING));
        paramParser.push_back(std::make_pair("stick_as_axis",       BOOL));
        paramParser.push_back(std::make_pair("gui_elements",        INT));

        for (auto& p : paramParser)
        {
            if (!cfg.check(p.first) || !(cfg.find(p.first).*isFunctionMap[p.second])())
            {
                std::string err_type = err_msgs.find(p.second) == err_msgs.end() ? "[unknown type]" : err_msgs[p.second];
                yError() << "ovrHeadset: parameter" << p.first << "not found or not" << err_type << "in configuration file";
                return false;
            }
        }
        guiCount = cfg.find("gui_elements").asInt32();
        paramParser.clear();
        if (guiCount)
        {
            paramParser.push_back(std::make_pair("width",  DOUBLE));
            paramParser.push_back(std::make_pair("height", DOUBLE));
            paramParser.push_back(std::make_pair("x",      DOUBLE));
            paramParser.push_back(std::make_pair("y",      DOUBLE));
            paramParser.push_back(std::make_pair("z",      DOUBLE));
            paramParser.push_back(std::make_pair("alpha",  DOUBLE));

            for (unsigned int i = 0; i < guiCount; ++i)
            {
                std::string       groupName  = "GUI_" + std::to_string(i);
                yarp::os::Bottle& guip       = cfg.findGroup(groupName);
                guiParam          hud;

                if (guip.isNull())
                {
                    yError() << "group:" << groupName << "not found in configuration file..";
                    return false;
                }

                for (auto& p : paramParser)
                {
                    if (!guip.check(p.first) || !(guip.find(p.first).*isFunctionMap[p.second])())
                    {
                        std::string err_type = err_msgs.find(p.second) == err_msgs.end() ? "[unknow type]" : err_msgs[p.second];
                        yError() << "ovrHeadset: parameter" << p.first << "not found or not" << err_type << "in" << groupName << "group in configuration file";
                        return false;
                    }
                }

                hud.resizeW = guip.find("width").asFloat64();
                hud.resizeH = guip.find("height").asFloat64();
                hud.x       = guip.find("x").asFloat64();
                hud.y       = guip.find("y").asFloat64();
                hud.z       = guip.find("z").asFloat64();
                hud.alpha   = guip.find("alpha").asFloat64();
                hud.port    = new FlexImagePort;
                hud.texture = new TextureBuffer();
                std::transform(groupName.begin(), groupName.end(), groupName.begin(), ::tolower);
                hud.port->open(standardPortPrefix + "/" + groupName);

                huds.push_back(hud);
            }
        }
        else
        {
            guiEnabled = false;
        }

    }

    getStickAsAxis = cfg.find("stick_as_axis").asBool();
    left_frame     = cfg.find("tf_left_hand_frame").asString();
    right_frame    = cfg.find("tf_right_hand_frame").asString();
    root_frame     = cfg.find("tf_root_frame").asString();
    relative       = cfg.check("hands_relative", yarp::os::Value(false)).asBool();

    //getting gui information from cfg

    fillAxisStorage();
    fillButtonStorage();
    fillErrorStorage();
    fillHatStorage();

    //opening tf client
    tfClientCfg.put("device", cfg.find("tfDevice").asString());
    tfClientCfg.put("local", cfg.find("tfLocal").asString());
    tfClientCfg.put("remote", cfg.find("tfRemote").asString());

    if (!driver.open(tfClientCfg))
    {
        yError() << "unable to open PolyDriver";
        return false;
    }

    if (!driver.view(tfPublisher) || tfPublisher == nullptr)
    {
        yError() << "unable to dynamic cast device to IFrameTransform interface";
        return false;
    }
    yInfo() << "TransformCLient successfully opened at port: " << cfg.find("tfLocal").asString();

    //opening ports
    ports =
    {
        { &orientationPort,                  "orientation"                  },
        { &positionPort,                     "position"                     },
        { &angularVelocityPort,              "angularVelocity"              },
        { &linearVelocityPort,               "linearVelocity"               },
        { &angularAccelerationPort,          "angularAcceleration"          },
        { &linearAccelerationPort,           "linearAcceleration"           },
        { &predictedOrientationPort,         "predictedOrientation"         },
        { &predictedPositionPort,            "predictedPosition"            },
        { &predictedAngularVelocityPort,     "predictedAngularVelocity"     },
        { &predictedLinearVelocityPort,      "predictedLinearVelocity"      },
        { &predictedAngularAccelerationPort, "predictedAngularAcceleration" },
        { &predictedLinearAccelerationPort,  "predictedLinearAcceleration"  }
    };

    for (auto port : ports)
    {
        std::string name, prefix;
        bool        predicted;

        *port.first = new yarp::os::BufferedPort<yarp::os::Bottle>;
        predicted   = port.second.find("predicted") != std::string::npos;
        prefix      = predicted ? standardPortPrefix+"/predicted" : standardPortPrefix;
        name        = prefix + "/headpose/" + port.second + ":o";

        if (!(*port.first)->open(name))
        {
            yError() << "Cannot open" << port.second << "port";
            this->close();
            return false;
        }

        (*port.first)->setWriteOnly();
    }

    //eyes set-up
    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        displayPorts[eye] = new InputCallback(eye);
        if (!displayPorts[eye]->open(eye == ovrEye_Left ? "/oculus/display/left:i" : "/oculus/display/right:i")) {
            yError() << "Cannot open " << (eye == ovrEye_Left ? "left" : "right") << "display port";
            this->close();
            return false;
        }
        displayPorts[eye]->setReadOnly();
    }

    texWidth  = cfg.check("w",    yarp::os::Value(640), "Texture width (usually same as camera width)").asInt32();
    texHeight = cfg.check("h",    yarp::os::Value(480), "Texture height (usually same as camera height)").asInt32();

    // TODO accept different fov for right and left eye?
    double hfov   = cfg.check("hfov", yarp::os::Value(105.),  "Camera horizontal field of view").asFloat64();
    camHFOV[0] = hfov;
    camHFOV[1] = hfov;

    //optional params
    optionalParams =
    {
        { "flipinput",     "[F] Enable input flipping",                &flipInputEnabled,  true },
        { "no-imagepose",  "[I] Disable image pose",                   &imagePoseEnabled,  false },
        { "userpose",      "[U] Use user pose instead of camera pose", &userPoseEnabled,   true  },
        { "no-logo",       "[L] Disable logo",                         &logoEnabled,       false },
        { "no-crosshairs", "[C] Disable crosshairs",                   &crosshairsEnabled, false },
        { "no-battery",    "[B] Disable battery",                      &batteryEnabled,    false }
    };

    for (auto p : optionalParams)
    {
        if (cfg.check(std::get<0>(p), std::get<1>(p)))
        {
            *std::get<2>(p) = std::get<3>(p);
        }
    }

    prediction = cfg.check("prediction", yarp::os::Value(0.01), "Prediction [sec]").asFloat64();

    displayPorts[0]->rollOffset  = static_cast<float>(cfg.check("left-roll-offset",   yarp::os::Value(0.0), "[LEFT_SHIFT+PAGE_UP][LEFT_SHIFT+PAGE_DOWN] Left eye roll offset").asFloat64());
    displayPorts[0]->pitchOffset = static_cast<float>(cfg.check("left-pitch-offset",  yarp::os::Value(0.0), "[LEFT_SHIFT+UP_ARROW][LEFT_SHIFT+DOWN_ARROW] Left eye pitch offset").asFloat64());
    displayPorts[0]->yawOffset   = static_cast<float>(cfg.check("left-yaw-offset",    yarp::os::Value(0.0), "[LEFT_SHIFT+LEFT_ARROW][LEFT_SHIFT+RIGHT_ARROW] Left eye yaw offset").asFloat64());
    displayPorts[1]->rollOffset  = static_cast<float>(cfg.check("right-roll-offset",  yarp::os::Value(0.0), "[RIGHT_SHIFT+PAGE_UP][RIGHT_SHIFT+PAGE_DOWN] Right eye roll offset").asFloat64());
    displayPorts[1]->pitchOffset = static_cast<float>(cfg.check("right-pitch-offset", yarp::os::Value(0.0), "[RIGHT_SHIFT+UP_ARROW][RIGHT_SHIFT+DOWN_ARROW] Right eye pitch offset").asFloat64());
    displayPorts[1]->yawOffset   = static_cast<float>(cfg.check("right-yaw-offset",   yarp::os::Value(0.0), "[RIGHT_SHIFT+LEFT_ARROW][RIGHT_SHIFT+RIGHT_ARROW] Right eye yaw offset").asFloat64());

    // Start the thread
    if (!this->start()) {
        yError() << "thread start failed, aborting.";
        this->close();
        return false;
    }

    // Enable display port callbacks
    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        displayPorts[eye]->useCallback();
    }

    return true;
}

bool yarp::dev::OVRHeadset::threadInit()
{
    yTrace();
    OVR::System::Init();

    // Initializes LibOVR, and the Rift
    ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, ovrDebugCallback, reinterpret_cast<uintptr_t>(this), 0 };
    ovrResult r = ovr_Initialize(&initParams);
//    VALIDATE(OVR_SUCCESS(r), "Failed to initialize libOVR.");
    if (!OVR_SUCCESS(r)) {
        yError() << "Failed to initialize libOVR.";
    }

    // Detect and initialize Oculus Rift
    ovrGraphicsLuid luid;
    ovrResult result = ovr_Create(&session, &luid);
    if (!OVR_SUCCESS(result)) {
        yError() << "Oculus Rift not detected.";
        this->close();
        return false;
    }

    if (compareLuid(luid, GetDefaultAdapterLuid())) // If luid that the Rift is on is not the default adapter LUID...
    {
        yError() << "OpenGL supports only the default graphics adapter.";
        this->close();
        return false;
    }

    // FIXME: Which one is better in this case?
    // ovrTrackingOrigin_FloorLevel will give tracking poses where the floor height is 0
    // ovrTrackingOrigin_EyeLevel will give tracking poses where the eye height is 0
    ovr_SetTrackingOriginType(session, ovrTrackingOrigin_EyeLevel);

    hmdDesc = ovr_GetHmdDesc(session);
    if (hmdDesc.ProductName[0] == '\0') {
        yWarning() << "Rift detected, display not enabled.";
    }

    DebugHmd(hmdDesc);

    // Initialize the GLFW system for creating and positioning windows
    // GLFW must be initialized after LibOVR
    // see http://www.glfw.org/docs/latest/rift.html
    if ( !glfwInit() ) {
        yError() << "Failed to initialize GLFW";
        this->close();
        return false;
    }
    glfwSetErrorCallback(glfwErrorCallback);

    OVR::Sizei windowSize = hmdDesc.Resolution;

    if (!createWindow(windowSize.w, windowSize.h)) {
        yError() << "Failed to create window";
        this->close();
        return false;
    }


    // Initialize the GLEW OpenGL 3.x bindings
    // GLEW must be initialized after creating the window
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        yError() << "glewInit failed, aborting.";
        this->close();
        return false;
    }
    yInfo() << "Using GLEW" << (const char*)glewGetString(GLEW_VERSION);
    checkGlErrorMacro;


    int fbwidth, fbheight;
    glfwGetFramebufferSize(window, &fbwidth, &fbheight);

    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        camWidth[eye] = texWidth;
        camHeight[eye] = texHeight;
    }
    reconfigureFOV();

    reconfigureRendering();

    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        displayPorts[eye]->eyeRenderTexture = new TextureBuffer(texWidth, texHeight, eye, session);
    }

    textureLogo = new TextureStatic(session, yarp_logo);
    textureCrosshairs = new TextureStatic(session, crosshairs);
    textureBattery = new TextureBattery(session, batteryEnabled);

    ovrMirrorTextureDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = windowSize.w;
    desc.Height = windowSize.h;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

    // Create mirror texture and an FBO used to copy mirror texture to back buffer
    result = ovr_CreateMirrorTextureGL(session, &desc, &mirrorTexture);
    if (!OVR_SUCCESS(result))
    {
        yError() << "Failed to create mirror texture.";
        this->close();
        return false;
    }

    // Configure the mirror read buffer
    GLuint texId;
    ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

    glGenFramebuffers(1, &mirrorFBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Recenter position
    ovr_RecenterTrackingOrigin(session);

    checkGlErrorMacro;

    return true;
}

void yarp::dev::OVRHeadset::threadRelease()
{
    yTrace();

    // Ensure that threadRelease is not called twice
    if (closed) {
        return;
    }
    closed = true;

    if (mirrorFBO) {
        glDeleteFramebuffers(1, &mirrorFBO);
    }

    if (mirrorTexture) {
        ovr_DestroyMirrorTexture(session, mirrorTexture);
    }

    if (textureLogo) {
        delete textureLogo;
        textureLogo = nullptr;
    }

    if (textureCrosshairs) {
        delete textureCrosshairs;
        textureCrosshairs = nullptr;
    }

    if (textureBattery) {
        delete textureBattery;
        textureBattery = nullptr;
    }

    // Shut down GLFW
    glfwTerminate();

    // Shut down LibOVR
    if (session) {
        // Disable Performance Hud Mode before destroying the session,
        // or it will stay after the device is closed.
        int PerfHudMode = (int)ovrPerfHud_Off;
        ovr_SetInt(session, OVR_PERF_HUD_MODE, PerfHudMode);

        ovr_Destroy(session);
        session = 0;
        ovr_Shutdown();
    }
    std::vector<yarp::os::Contactable*> ports;

    ports.push_back(orientationPort);
    ports.push_back(positionPort);
    ports.push_back(angularVelocityPort);
    ports.push_back(linearVelocityPort);
    ports.push_back(angularAccelerationPort);
    ports.push_back(linearAccelerationPort);
    ports.push_back(predictedOrientationPort);
    ports.push_back(predictedPositionPort);
    ports.push_back(predictedAngularVelocityPort);
    ports.push_back(predictedLinearVelocityPort);
    ports.push_back(predictedAngularAccelerationPort);
    ports.push_back(predictedLinearAccelerationPort);

    for (auto& hud : huds)
    {
        delete hud.texture;
        ports.push_back(hud.port);
    }

    for (auto& p : ports)
    {
        if (p != nullptr)
        {
            p->interrupt();
            p->close();
            delete p;
            p = nullptr;
        }
    }


    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        if (displayPorts[eye]) {
            displayPorts[eye]->disableCallback();
            displayPorts[eye]->interrupt();
            displayPorts[eye]->close();
            delete displayPorts[eye];
            displayPorts[eye] = nullptr;
        }
    }
}

bool yarp::dev::OVRHeadset::close()
{
    yTrace();
    this->askToStop();
    return true;
}

bool yarp::dev::OVRHeadset::startService()
{
    yTrace();
    return false;
}

bool yarp::dev::OVRHeadset::updateService()
{
    if (closed) {
        return false;
    }

    constexpr double delay = 60.0;
    yDebug("Thread ran %d times, est period %lf[ms], used %lf[ms]",
           getIterations(),
           getEstimatedPeriod()*1000,
           getEstimatedUsed()*1000);
    yDebug("Display refresh: %3.1f[hz]", getIterations()/delay);

    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        yDebug("%s eye: %3.1f[hz] - %d of %d frames missing, %d of %d frames dropped",
               (eye == ovrEye_Left ? "Left " : "Right"),
               (getIterations() - displayPorts[eye]->eyeRenderTexture->missingFrames) / delay,
               displayPorts[eye]->eyeRenderTexture->missingFrames,
               getIterations(),
               displayPorts[eye]->droppedFrames,
               getIterations());
        displayPorts[eye]->eyeRenderTexture->missingFrames = 0;
        getIterations(),
        displayPorts[eye]->droppedFrames = 0;
    }

    resetStat();

    yarp::os::SystemClock::delaySystem(delay);
    return !closed;
}

bool yarp::dev::OVRHeadset::stopService()
{
    yTrace();
    return this->close();
}

void yarp::dev::OVRHeadset::resetInput()
{
    inputStateMutex.lock();
    inputState.Buttons = 0;
    inputState.HandTrigger[0] = 0;
    inputState.HandTrigger[1] = 0;
    inputState.IndexTrigger[0] = 0;
    inputState.IndexTrigger[1] = 0;
    inputState.Thumbstick[0].x = 0;
    inputState.Thumbstick[0].y = 0;
    inputState.Thumbstick[1].x = 0;
    inputState.Thumbstick[1].y = 0;
    inputStateMutex.unlock();
}

void yarp::dev::OVRHeadset::run()
{
    ovrResult        result = ovrError_InvalidSession;
    ovrSessionStatus sessionStatus;

    if (glfwWindowShouldClose(window)) {
        resetInput();
        close();
        return;
    }

    ovr_GetSessionStatus(session, &sessionStatus);
    if (sessionStatus.ShouldQuit) {
        resetInput();
        close();
        return;
    }
    if (sessionStatus.ShouldRecenter) {
        ovr_RecenterTrackingOrigin(session);
    }

    // Check window events;
    glfwPollEvents();

    if (!sessionStatus.IsVisible) {
        resetInput();
        return;
    }

    if (!sessionStatus.HasInputFocus) {
      //  return;
    }

    // Begin frame
    ++distortionFrameIndex;
    double frameTiming = ovr_GetPredictedDisplayTime(session, distortionFrameIndex);
    YARP_UNUSED(frameTiming);

    // Query the HMD for the current tracking state.
    ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds(), false);
    headpose = ts.HeadPose;
    yarp::os::Stamp stamp(distortionFrameIndex, ts.HeadPose.TimeInSeconds);

    //Get eye poses, feeding in correct IPD offset
    ovrPosef ViewPose[2] = {EyeRenderDesc[0].HmdToEyePose,EyeRenderDesc[1].HmdToEyePose};
    ovrPosef EyeRenderPose[2];
    ovr_CalcEyePoses(headpose.ThePose, ViewPose, EyeRenderPose);

    // Query the HMD for the predicted state
    ovrTrackingState predicted_ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds() + prediction, false);
    ovrPoseStatef predicted_headpose = predicted_ts.HeadPose;
    yarp::os::Stamp predicted_stamp(distortionFrameIndex, predicted_ts.HeadPose.TimeInSeconds);

    //send hands frames
    if (relative)
    {
        yarp::sig::Matrix T_Conv(4, 4), T_Head(4, 4), T_LHand(4, 4), T_RHand(4, 4), T_robotHead(4, 4);
        yarp::sig::Vector rpyHead, rpyRobot;

        tfPublisher->getTransform("head_link", "mobile_base_body_link", T_robotHead);
        ovrVector3f& leftH = ts.HandPoses[ovrHand_Left].ThePose.Position;
        ovrVector3f& rightH = ts.HandPoses[ovrHand_Right].ThePose.Position;

        T_RHand    = ovr2matrix(vecSubtract(rightH, headpose.ThePose.Position), OVR::Quatf(ts.HandPoses[ovrHand_Right].ThePose.Orientation) * OVR::Quatf(OVR::Vector3f(0, 0, 1), M_PI_2));
        T_LHand    = ovr2matrix(vecSubtract(leftH, headpose.ThePose.Position), OVR::Quatf(ts.HandPoses[ovrHand_Left].ThePose.Orientation)   * OVR::Quatf(OVR::Vector3f(0, 0, 1), M_PI_2));
        T_Head     = ovr2matrix(headpose.ThePose.Position, headpose.ThePose.Orientation);
        rpyHead    = yarp::math::dcm2rpy(T_Head);
        rpyRobot   = yarp::math::dcm2rpy(T_robotHead);
        rpyHead[0] = 0;
        rpyHead[1] = rpyRobot[1];
        rpyHead[2] = rpyRobot[2];
        T_Head     = yarp::math::rpy2dcm(rpyHead);

        tfPublisher->setTransform(left_frame,    root_frame, operator*(T_Head.transposed(), T_LHand));
        tfPublisher->setTransform(right_frame,   root_frame, operator*(T_Head.transposed(), T_RHand));
    }

    else

    {
        OVR::Quatf lRot = OVR::Quatf(ts.HandPoses[ovrHand_Left].ThePose.Orientation)  * OVR::Quatf(OVR::Vector3f(0, 0, 1), M_PI_2);
        OVR::Quatf rRot = OVR::Quatf(ts.HandPoses[ovrHand_Right].ThePose.Orientation) * OVR::Quatf(OVR::Vector3f(0, 0, 1), M_PI_2);
        tfPublisher->setTransform(left_frame, "mobile_base_body_link", ovr2matrix(ts.HandPoses[ovrHand_Left].ThePose.Position,   lRot));
        tfPublisher->setTransform(right_frame, "mobile_base_body_link", ovr2matrix(ts.HandPoses[ovrHand_Right].ThePose.Position, rRot));
    }

    //tfPublisher->setTransform(right_frame,   root_frame, yarp::math::operator*(T_Head.transposed(), T_RHand));

    // Get Input State
    inputStateMutex.lock();
    result = ovr_GetInputState(session, ovrControllerType_Active, &inputState);
    inputStateMutex.unlock();
    if (!OVR_SUCCESS(result))
    {
        errorManager(result);
        inputStateError = true;
    }

    // Read orientation and write it on the port
    if (ts.StatusFlags & ovrStatus_OrientationTracked) {

        if (orientationPort->getOutputCount() > 0) {
            OVR::Quatf orientation = headpose.ThePose.Orientation;
            float yaw, pitch, roll;
            orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
            yarp::os::Bottle& output_orientation = orientationPort->prepare();
            output_orientation.clear();
            output_orientation.addFloat64(OVR::RadToDegree(pitch));
            output_orientation.addFloat64(OVR::RadToDegree(-roll));
            output_orientation.addFloat64(OVR::RadToDegree(yaw));
            orientationPort->setEnvelope(stamp);
            orientationPort->write();
        }

        writeVec3OnPort(angularVelocityPort,     radToDeg(headpose.AngularVelocity),     stamp);
        writeVec3OnPort(angularAccelerationPort, radToDeg(headpose.AngularAcceleration), stamp);

    } else {
        // Do not warn more than once every 5 seconds
        static double lastOrientWarnTime = 0;
        double now = yarp::os::SystemClock::nowSystem();
        if (now >= lastOrientWarnTime + 5) {
            yDebug() << "Orientation not tracked";
            lastOrientWarnTime = now;
        }
    }

    // Read position and write it on the port
    if (ts.StatusFlags & ovrStatus_PositionTracked) {
        writeVec3OnPort(positionPort,           headpose.ThePose.Position,   stamp);
        writeVec3OnPort(linearVelocityPort,     headpose.LinearVelocity,     stamp);
        writeVec3OnPort(linearAccelerationPort, headpose.LinearAcceleration, stamp);

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPosWarnTime = 0;
        double now = yarp::os::SystemClock::nowSystem();
        if (now >= lastPosWarnTime + 5) {
            yDebug() << "Position not tracked";
            lastPosWarnTime = now;
        }
    }

    // Read predicted orientation and write it on the port
    if (predicted_ts.StatusFlags & ovrStatus_OrientationTracked) {

        if (predictedOrientationPort->getOutputCount() > 0) {
            OVR::Quatf orientation = predicted_headpose.ThePose.Orientation;
            float yaw, pitch, roll;
            orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
            yarp::os::Bottle& output_orientation = predictedOrientationPort->prepare();
            output_orientation.clear();
            output_orientation.addFloat64(OVR::RadToDegree(pitch));
            output_orientation.addFloat64(OVR::RadToDegree(-roll));
            output_orientation.addFloat64(OVR::RadToDegree(yaw));
            predictedOrientationPort->setEnvelope(predicted_stamp);
            predictedOrientationPort->write();
        }

        writeVec3OnPort(predictedAngularVelocityPort,     radToDeg(predicted_headpose.AngularVelocity),     stamp);
        writeVec3OnPort(predictedAngularAccelerationPort, radToDeg(predicted_headpose.AngularAcceleration), stamp);

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPredOrientWarnTime = 0;
        double now = yarp::os::SystemClock::nowSystem();
        if (now >= lastPredOrientWarnTime + 5) {
            yDebug() << "Predicted orientation not tracked";
            lastPredOrientWarnTime = now;
        }
    }

    // Read predicted position and write it on the port
    if (predicted_ts.StatusFlags & ovrStatus_PositionTracked) {

        writeVec3OnPort(predictedPositionPort,           predicted_headpose.ThePose.Position,   stamp);
        writeVec3OnPort(predictedLinearVelocityPort,     predicted_headpose.LinearVelocity,     stamp);
        writeVec3OnPort(predictedLinearAccelerationPort, predicted_headpose.LinearAcceleration, stamp);

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPredPosWarnTime = 0;
        double now = yarp::os::SystemClock::nowSystem();
        if (now >= lastPredPosWarnTime + 5) {
            yDebug() << "Position not tracked";
            lastPredPosWarnTime = now;
        }
    }


    if (displayPorts[0]->eyeRenderTexture && displayPorts[1]->eyeRenderTexture) {
        // Do distortion rendering, Present and flush/sync

        // Update the textures
        for (int eye = 0; eye < ovrEye_Count; ++eye) {
            displayPorts[eye]->eyeRenderTexture->update();
        }


        for (int eye = 0; eye < ovrEye_Count; ++eye) {
            if (imagePoseEnabled) {
                if (userPoseEnabled) {
                    // Use orientation read from the HMD at the beginning of the frame
                    EyeRenderPose[eye].Orientation = headpose.ThePose.Orientation;
                } else {
                    // Use orientation received from the image
                    EyeRenderPose[eye].Orientation = displayPorts[eye]->eyeRenderTexture->eyePose.Orientation;
                }
            } else {
                EyeRenderPose[eye].Orientation.w = -1.0f;
                EyeRenderPose[eye].Orientation.x = 0.0f;
                EyeRenderPose[eye].Orientation.y = 0.0f;
                EyeRenderPose[eye].Orientation.z = 0.0f;
            }
        }

        // If the image size is different from the texture size,
        bool needReconfigureFOV = false;
        for (int eye = 0; eye < ovrEye_Count; ++eye) {
            if ((displayPorts[eye]->eyeRenderTexture->imageWidth != 0 && displayPorts[eye]->eyeRenderTexture->imageWidth != camWidth[eye]) ||
                (displayPorts[eye]->eyeRenderTexture->imageHeight != 0 && displayPorts[eye]->eyeRenderTexture->imageHeight != camHeight[eye])) {

                camWidth[eye] = displayPorts[eye]->eyeRenderTexture->imageWidth;
                camHeight[eye] = displayPorts[eye]->eyeRenderTexture->imageHeight;
                needReconfigureFOV = true;
            }
        }
        if (needReconfigureFOV) {
            reconfigureFOV();
            reconfigureRendering();
        }

        std::list<ovrLayerHeader*> layerList;

        ovrLayerEyeFov eyeLayer;
        eyeLayer.Header.Type = ovrLayerType_EyeFov;
        eyeLayer.Header.Flags = ovrLayerFlag_HighQuality;
        if (flipInputEnabled) {
            eyeLayer.Header.Flags |= ovrLayerFlag_TextureOriginAtBottomLeft;
        }
        for (int eye = 0; eye < 2; ++eye) {
            eyeLayer.ColorTexture[eye] = displayPorts[eye]->eyeRenderTexture->textureSwapChain;
            eyeLayer.Viewport[eye] = OVR::Recti(0, 0, displayPorts[eye]->eyeRenderTexture->width, displayPorts[eye]->eyeRenderTexture->height);
            eyeLayer.Fov[eye] = fov[eye];
            eyeLayer.RenderPose[eye] = EyeRenderPose[eye];
        }
        layerList.push_back(&eyeLayer.Header);

        if (logoEnabled) {
            setHeadLockedLayer(logoLayer, textureLogo, 0.2f, -0.2f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.05f, 0.05f);
            layerList.push_back(&logoLayer.Header);
        }

        if (crosshairsEnabled) {
            setHeadLockedLayer(crosshairsLayer, textureCrosshairs, 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.08f, 0.08f);
            layerList.push_back(&crosshairsLayer.Header);
        }

        if (batteryEnabled) {
            setHeadLockedLayer(batteryLayer, textureBattery->currentTexture, 0.25f, 0.25f, -0.50f, 0.0f, 0.0f, 0.0f, 1.0f, 0.05f, 0.05f);
            layerList.push_back(&batteryLayer.Header);
        }

        //setting up dynamic hud
        if (guiEnabled)
        {
            for (auto& hud : huds)
            {
                if (!hud.port->getInputCount())
                {
                    continue;
                }

                yarp::sig::FlexImage* image = hud.port->read(false);

                if (!image)
                {
                    layerList.push_back(&hud.layer.Header);
                    continue;
                }

                hud.texture->fromImage(session, *image, hud.alpha);
                setHeadLockedLayer(hud.layer, hud.texture, hud.x, hud.y, hud.z, 0.0f, 0.0f, 0.0f, 1.0f, hud.resizeW, hud.resizeH);
                layerList.push_back(&hud.layer.Header);
            }
        }

        ovrLayerHeader** layers = new ovrLayerHeader*[layerList.size()];
        std::copy(layerList.begin(), layerList.end(), layers);

        ovr_WaitToBeginFrame(session, distortionFrameIndex);
        ovr_BeginFrame(session, distortionFrameIndex);
        ovr_EndFrame(session, distortionFrameIndex, nullptr, layers, layerList.size());
        delete[] layers;

        // Blit mirror texture to back buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        GLint bw = hmdDesc.Resolution.w;
        GLint bh = hmdDesc.Resolution.h;
        GLint ww, wh;
        glfwGetWindowSize(window, &ww, &wh);
        glBlitFramebuffer(0, bh, bw, 0, 0, 0, ww, wh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        checkGlErrorMacro;

        glfwSwapBuffers(window);

    } else {
        // Do not warn more than once every 5 seconds
        static double lastImgWarnTime = 0;
        double now = yarp::os::SystemClock::nowSystem();
        if (now >= lastImgWarnTime + 5) {
            yDebug() << "No image received";
            lastImgWarnTime = now;
        }
    }
}

bool yarp::dev::OVRHeadset::createWindow(int w, int h)
{
    yTrace();
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    window = glfwCreateWindow(w/2, h/2, "YARP Oculus", nullptr, nullptr);
    if (!window) {
        yError() << "Could not create window";
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, glfwKeyCallback);
    glfwMakeContextCurrent(window);

    return true;
}

void yarp::dev::OVRHeadset::onKey(int key, int scancode, int action, int mods)
{
    yTrace();

    if (GLFW_PRESS != action) {
        return;
    }

    bool leftShiftPressed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool rightShiftPressed = (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    bool leftCtrlPressed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
    bool rightCtrlPressed = (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
    bool leftAltPressed = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);
    bool rightAltPressed = (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
    bool shiftPressed = leftShiftPressed || rightShiftPressed;
    bool ctrlPressed = leftCtrlPressed || rightCtrlPressed;
    bool altPressed = leftAltPressed || rightAltPressed;

    switch (key) {
    case GLFW_KEY_R:

        if (!leftShiftPressed && !rightShiftPressed) {
            yDebug() << "Recentering pose";
            ovr_RecenterTrackingOrigin(session);
        } else {
            yDebug() << "Resetting yaw offset to current position";
            for (int eye = 0; eye < ovrEye_Count; ++eye) {
                float iyaw, ipitch, iroll;
                if (imagePoseEnabled) {
                    OVR::Quatf imageOrientation = displayPorts[eye]->eyeRenderTexture->eyePose.Orientation;
                    imageOrientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&iyaw, &ipitch, &iroll);
                } else {
                    iyaw = 0.0f;
                    ipitch = 0.0f;
                    iyaw = 0.0f;
                }

                iyaw -= displayPorts[eye]->yawOffset;
                displayPorts[eye]->yawOffset = - iyaw;
                yDebug() << (eye == ovrEye_Left? "Left" : "Right") << "eye yaw offset =" << displayPorts[eye]->yawOffset;
            }
        }
        break;
    case GLFW_KEY_F:
        flipInputEnabled = !flipInputEnabled;
        yDebug() << "Flip input" << (flipInputEnabled ? "ON" : "OFF");
        reconfigureRendering();
        break;
    case GLFW_KEY_I:
        imagePoseEnabled = !imagePoseEnabled;
        yDebug() << "Image pose" << (imagePoseEnabled ? "ON" : "OFF");
        yDebug() << "User pose" << (userPoseEnabled ? "ON" : "OFF");
        break;
    case GLFW_KEY_U:
        userPoseEnabled = !userPoseEnabled;
        yDebug() << "Image pose" << (imagePoseEnabled ? "ON" : "OFF");
        yDebug() << "User pose" << (userPoseEnabled ? "ON" : "OFF");
        break;
    case GLFW_KEY_L:
        logoEnabled = !logoEnabled;
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF") <<
            "Gui" << ((guiCount != 0) ? (guiEnabled ? "ON" : "OFF") : "DISABLED");
        break;
    case GLFW_KEY_C:
        crosshairsEnabled = !crosshairsEnabled;
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF") <<
            "Gui" << ((guiCount != 0) ? (guiEnabled ? "ON" : "OFF") : "DISABLED");
        break;
    case GLFW_KEY_B:
        batteryEnabled = !batteryEnabled;
        if (batteryEnabled) {
            textureBattery->resume();
        } else {
            textureBattery->suspend();
        }
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF") <<
            "Gui" << ((guiCount != 0) ? (guiEnabled ? "ON" : "OFF") : "DISABLED");
        break;
    case GLFW_KEY_G:
        if (guiCount != 0) {
            guiEnabled = !guiEnabled;
        }
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF") <<
            "Gui" << ((guiCount != 0) ? (guiEnabled ? "ON" : "OFF") : "DISABLED") ;
        break;
    case GLFW_KEY_ESCAPE:
        this->close();
        break;
    case GLFW_KEY_Z:
        if (!rightShiftPressed) {
            --camHFOV[0];
            yDebug() << "Left eye HFOV =" << camHFOV[0];
        }
        if (!leftShiftPressed) {
            --camHFOV[1];
            yDebug() << "Right eye HFOV =" << camHFOV[1];
        }
        reconfigureFOV();
        reconfigureRendering();
        break;
    case GLFW_KEY_X:
        if (!rightShiftPressed) {
            ++camHFOV[0];
            yDebug() << "Left eye HFOV =" << camHFOV[0];
        }
        if (!leftShiftPressed) {
            ++camHFOV[1];
            yDebug() << "Right eye HFOV =" << camHFOV[1];
        }
        reconfigureFOV();
        reconfigureRendering();
        break;
    case GLFW_KEY_UP:
        if (!rightShiftPressed) {
            displayPorts[0]->pitchOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye pitch offset =" << displayPorts[0]->pitchOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->pitchOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye pitch offset =" << displayPorts[1]->pitchOffset;
        }
        break;
    case GLFW_KEY_DOWN:
        if (!rightShiftPressed) {
            displayPorts[0]->pitchOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye pitch offset =" << displayPorts[0]->pitchOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->pitchOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye pitch offset =" << displayPorts[1]->pitchOffset;
        }
        break;
    case GLFW_KEY_LEFT:
        if (!rightShiftPressed) {
            displayPorts[0]->yawOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye yaw offset =" << displayPorts[0]->yawOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->yawOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye yaw offset =" << displayPorts[1]->yawOffset;
        }
        break;
    case GLFW_KEY_RIGHT:
        if (!rightShiftPressed) {
            displayPorts[0]->yawOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye yaw offset =" << displayPorts[0]->yawOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->yawOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye yaw offset =" << displayPorts[1]->yawOffset;
        }
        break;
    case GLFW_KEY_PAGE_UP:
        if (!rightShiftPressed) {
            displayPorts[0]->rollOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye roll offset =" << displayPorts[0]->rollOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->rollOffset += ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye roll offset =" << displayPorts[1]->rollOffset;
        }
        break;
    case GLFW_KEY_PAGE_DOWN:
        if (!rightShiftPressed) {
            displayPorts[0]->rollOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye roll offset =" << displayPorts[0]->rollOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->rollOffset -= ctrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye roll offset =" << displayPorts[1]->rollOffset;
        }
        break;
    case GLFW_KEY_SLASH:
        {
            int PerfHudMode = ovr_GetInt(session, OVR_PERF_HUD_MODE, 0);
            PerfHudMode = (PerfHudMode + 1) % 8;
            ovr_SetInt(session, OVR_PERF_HUD_MODE, PerfHudMode);
        }
        break;
    case GLFW_KEY_P:
        yDebug() << "--------------------------------------------";
        yDebug() << "Current settings:";
        yDebug() << "  Flip input" << (flipInputEnabled ? "ON" : "OFF");
        yDebug() << "  Image pose" << (imagePoseEnabled ? "ON" : "OFF");
        yDebug() << "  User pose" << (userPoseEnabled ? "ON" : "OFF");
        yDebug() << "  Overlays:";
        yDebug() << "    Logo" << (logoEnabled ? "ON" : "OFF");
        yDebug() << "    Crosshairs" << (crosshairsEnabled ? "ON" : "OFF");
        yDebug() << "    Battery" << (batteryEnabled ? "ON" : "OFF");
        yDebug() << "    Gui" << ((guiCount != 0) ? (guiEnabled ? "ON" : "OFF") : "DISABLED");
        yDebug() << "  Left eye:";
        yDebug() << "    HFOV = " << camHFOV[0];
        yDebug() << "    pitch offset =" << displayPorts[0]->pitchOffset;
        yDebug() << "    yaw offset =" << displayPorts[0]->yawOffset;
        yDebug() << "    roll offset =" << displayPorts[0]->rollOffset;
        yDebug() << "  Right eye:";
        yDebug() << "    HFOV =" << camHFOV[1];
        yDebug() << "    pitch offset =" << displayPorts[1]->pitchOffset;
        yDebug() << "    yaw offset =" << displayPorts[1]->yawOffset;
        yDebug() << "    roll offset =" << displayPorts[1]->rollOffset;
        yDebug() << "--------------------------------------------";
        break;
    default:
        break;
    }
}

void yarp::dev::OVRHeadset::reconfigureRendering()
{
    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        ovr_GetRenderDesc(session, (ovrEyeType)eye, fov[eye]);
    }
}

void yarp::dev::OVRHeadset::reconfigureFOV()
{
    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        double camHFOV_rad = OVR::DegreeToRad(camHFOV[eye]);
        double texCamRatio = static_cast<double>(texWidth)/camWidth[eye];
        double texHFOV_rad = 2 * (atan(texCamRatio * tan(camHFOV_rad/2)));

        double aspectRatio = static_cast<double>(texWidth)/texHeight;
        fov[eye].UpTan    = static_cast<float>(fabs(tan(texHFOV_rad/2)/aspectRatio));
        fov[eye].DownTan  = static_cast<float>(fabs(tan(texHFOV_rad/2)/aspectRatio));
        fov[eye].LeftTan  = static_cast<float>(fabs(tan(texHFOV_rad/2)));
        fov[eye].RightTan = static_cast<float>(fabs(tan(texHFOV_rad/2)));
    }
    debugFov(fov);
}

void yarp::dev::OVRHeadset::glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    OVRHeadset* instance = (OVRHeadset*)glfwGetWindowUserPointer(window);
    instance->onKey(key, scancode, action, mods);
}

void yarp::dev::OVRHeadset::glfwErrorCallback(int error, const char* description)
{
    yError() << error << description;
}

void yarp::dev::OVRHeadset::ovrDebugCallback(uintptr_t userData, int level, const char* message)
{
    yarp::dev::OVRHeadset* ovr = reinterpret_cast<yarp::dev::OVRHeadset*>(userData);
    YARP_UNUSED(ovr);

    if (!message)
    {
        return;
    }

    switch (level) {
    case ovrLogLevel_Debug:
        yDebug() << "ovrDebugCallback" << message;
        break;
    case ovrLogLevel_Info:
        yInfo() << "ovrDebugCallback" << message;
        break;
    case ovrLogLevel_Error:
        yError() << "ovrDebugCallback" << message;
        break;
    default:
        yWarning() << "ovrDebugCallback" << message;
        break;
    }
}

void yarp::dev::OVRHeadset::DebugHmd(ovrHmdDesc hmdDesc)
{
    yDebug("  * ProductName: %s", hmdDesc.ProductName);
    yDebug("  * Manufacturer: %s", hmdDesc.Manufacturer);
    yDebug("  * VendorId:ProductId: %04X:%04X", hmdDesc.VendorId, hmdDesc.ProductId);
    yDebug("  * SerialNumber: %s", hmdDesc.SerialNumber);
    yDebug("  * Firmware Version: %d.%d", hmdDesc.FirmwareMajor, hmdDesc.FirmwareMinor);
    yDebug("  * Resolution: %dx%d", hmdDesc.Resolution.w, hmdDesc.Resolution.h);
}

void yarp::dev::OVRHeadset::errorManager(ovrResult error)
{
    if (error_messages.find(error) != error_messages.end())
    {
        yError() << error_messages[error].c_str();
    }
}


//IJoypadController method
bool yarp::dev::OVRHeadset::getAxisCount(unsigned int& axis_count)
{
    if (inputStateError) return false;
    axis_count = axisIdToValue.size();
    return true;
}

bool yarp::dev::OVRHeadset::getButtonCount(unsigned int& button_count)
{
    if (inputStateError) return false;
    button_count = BUTTON_COUNT;
    return true;
}

bool yarp::dev::OVRHeadset::getTrackballCount(unsigned int& Trackball_count)
{
    if (inputStateError) return false;
    Trackball_count = 0;
    return true;
}

bool yarp::dev::OVRHeadset::getHatCount(unsigned int& Hat_count)
{
    if (inputStateError) return false;
    Hat_count = 1;
    return true;
}

bool yarp::dev::OVRHeadset::getTouchSurfaceCount(unsigned int& touch_count)
{
    if (inputStateError) return false;
    touch_count = 0;
    return true;
}

bool yarp::dev::OVRHeadset::getStickCount(unsigned int& stick_count)
{
    if (inputStateError) return false;
    stick_count = getStickAsAxis ? 0 : STICK_COUNT;
    return true;
}

bool yarp::dev::OVRHeadset::getStickDoF(unsigned int stick_id, unsigned int& DoF)
{
    DoF = 2;
    return true;
}

bool yarp::dev::OVRHeadset::getButton(unsigned int button_id, float& value)
{
    if (inputStateError) return false;
    std::lock_guard<std::mutex> lock(inputStateMutex);
    if (button_id > buttonIdToOvrButton.size() - 1)
    {
        yError() << "OVRHeadset: button id out of bound";
        return false;
    }
    value = inputState.Buttons & buttonIdToOvrButton[button_id] ? 1.0f : 0.0f;
    return true;
}

bool yarp::dev::OVRHeadset::getTrackball(unsigned int trackball_id, yarp::sig::Vector& value)
{
    return false;
}

bool yarp::dev::OVRHeadset::getHat(unsigned int hat_id, unsigned char& value)
{
    if (inputStateError) return false;
    std::lock_guard<std::mutex> lock(inputStateMutex);
    if (hat_id > 0)
    {
        yError() << "OVRHeadset: hat id out of bound";
        return false;
    }
    value = DButtonToHat[inputState.Buttons & ovrButton_Up]    |
            DButtonToHat[inputState.Buttons & ovrButton_Down]  |
            DButtonToHat[inputState.Buttons & ovrButton_Right] |
            DButtonToHat[inputState.Buttons & ovrButton_Left];
    return true;
}

bool yarp::dev::OVRHeadset::getAxis(unsigned int axis_id, double& value)
{
    std::lock_guard<std::mutex> lock(inputStateMutex);
    if (axis_id > axisIdToValue.size())
    {
        yError() << "OVRHeadset: axis id out of bound";
        return false;
    }

    value = *axisIdToValue[axis_id];
    return true;
}

bool yarp::dev::OVRHeadset::getStick(unsigned int stick_id, yarp::sig::Vector& value, JoypadCtrl_coordinateMode coordinate_mode)
{
    if (inputStateError) return false;
    std::lock_guard<std::mutex> lock(inputStateMutex);
    if (getStickAsAxis)
    {
        return false;
    }

    if (stick_id > STICK_COUNT - 1)
    {
        yError() << "stick id out of bound";
        return false;
    }
    value.clear();
    if (coordinate_mode == JoypadCtrl_coordinateMode::JypCtrlcoord_POLAR)
    {
        value.push_back(sqrt(inputState.Thumbstick[stick_id].y * inputState.Thumbstick[stick_id].y +
                             inputState.Thumbstick[stick_id].x * inputState.Thumbstick[stick_id].x));

        value.push_back(atan2(inputState.Thumbstick[stick_id].y, inputState.Thumbstick[stick_id].x));
    }
    value.push_back(inputState.Thumbstick[stick_id].x);
    value.push_back(inputState.Thumbstick[stick_id].y);
    return true;
}

bool yarp::dev::OVRHeadset::getTouch(unsigned int touch_id, yarp::sig::Vector& value)
{
    return false;
}
