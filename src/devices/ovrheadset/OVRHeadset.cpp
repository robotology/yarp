/*
 * Copyright (C) 2015-2017  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


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

#include <math.h>

#include <OVR_CAPI_Util.h>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif

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

static void debugFov(const ovrFovPort fov[2]) {
    yDebug("             Left Eye                                           Right Eye\n");
    yDebug("LeftTan    %10f (%5f[rad] = %5f[deg])        %10f (%5f[rad] = %5f[deg])\n",
           fov[0].LeftTan,
           atan(fov[0].LeftTan),
           OVR::RadToDegree(atan(fov[0].LeftTan)),
           fov[1].LeftTan,
           atan(fov[1].LeftTan),
           OVR::RadToDegree(atan(fov[1].LeftTan)));
    yDebug("RightTan   %10f (%5f[rad] = %5f[deg])        %10f (%5f[rad] = %5f[deg])\n",
           fov[0].RightTan,
           atan(fov[0].RightTan),
           OVR::RadToDegree(atan(fov[0].RightTan)),
           fov[1].RightTan,
           atan(fov[1].RightTan),
           OVR::RadToDegree(atan(fov[1].RightTan)));
    yDebug("UpTan      %10f (%5f[rad] = %5f[deg])        %10f (%5f[rad] = %5f[deg])\n",
           fov[0].UpTan,
           atan(fov[0].UpTan),
           OVR::RadToDegree(atan(fov[0].UpTan)),
           fov[1].UpTan,
           atan(fov[1].UpTan),
           OVR::RadToDegree(atan(fov[1].UpTan)));
    yDebug("DownTan    %10f (%5f[rad] = %5f[deg])        %10f (%5f[rad] = %5f[deg])\n",
           fov[0].DownTan,
           atan(fov[0].DownTan),
           OVR::RadToDegree(atan(fov[0].DownTan)),
           fov[1].DownTan,
           atan(fov[0].DownTan),
           OVR::RadToDegree(atan(fov[0].DownTan)));
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

yarp::dev::OVRHeadset::OVRHeadset() :
        yarp::dev::DeviceDriver(),
        yarp::os::RateThread(11), // ~90 fps
        orientationPort(nullptr),
        positionPort(nullptr),
        angularVelocityPort(nullptr),
        linearVelocityPort(nullptr),
        angularAccelerationPort(nullptr),
        linearAccelerationPort(nullptr),
        predictedOrientationPort(nullptr),
        predictedPositionPort(nullptr),
        predictedAngularVelocityPort(nullptr),
        predictedLinearVelocityPort(nullptr),
        predictedAngularAccelerationPort(nullptr),
        predictedLinearAccelerationPort(nullptr),
        displayPorts{ nullptr, nullptr },
        textureLogo(nullptr),
        textureCrosshairs(nullptr),
        textureBattery(nullptr),
        mirrorTexture(nullptr),
        mirrorFBO(0),
        window(nullptr),
        closed(false),
        distortionFrameIndex(0),
        flipInputEnabled(false),
        imagePoseEnabled(true),
        userPoseEnabled(false),
        logoEnabled(true),
        crosshairsEnabled(true),
        batteryEnabled(true)
{
    yTrace();
}

yarp::dev::OVRHeadset::~OVRHeadset()
{
    yTrace();
}

bool yarp::dev::OVRHeadset::open(yarp::os::Searchable& cfg)
{
    yTrace();

    orientationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!orientationPort->open("/oculus/headpose/orientation:o")) {
        yError() << "Cannot open orientation port";
        this->close();
        return false;
    }
    orientationPort->setWriteOnly();

    positionPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!positionPort->open("/oculus/headpose/position:o")) {
        yError() << "Cannot open position port";
        this->close();
        return false;
    }
    positionPort->setWriteOnly();

    angularVelocityPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!angularVelocityPort->open("/oculus/headpose/angularVelocity:o")) {
        yError() << "Cannot open angular velocity port";
        this->close();
        return false;
    }
    angularVelocityPort->setWriteOnly();

    linearVelocityPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!linearVelocityPort->open("/oculus/headpose/linearVelocity:o")) {
        yError() << "Cannot open linear velocity port";
        this->close();
        return false;
    }
    linearVelocityPort->setWriteOnly();

    angularAccelerationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!angularAccelerationPort->open("/oculus/headpose/angularAcceleration:o")) {
        yError() << "Cannot open angular acceleration port";
        this->close();
        return false;
    }
    angularAccelerationPort->setWriteOnly();

    linearAccelerationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!linearAccelerationPort->open("/oculus/headpose/linearAcceleration:o")) {
        yError() << "Cannot open linear acceleration port";
        this->close();
        return false;
    }
    linearAccelerationPort->setWriteOnly();


    predictedOrientationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedOrientationPort->open("/oculus/predicted/headpose/orientation:o")) {
        yError() << "Cannot open predicted orientation port";
        this->close();
        return false;
    }
    predictedOrientationPort->setWriteOnly();

    predictedPositionPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedPositionPort->open("/oculus/predicted/headpose/position:o")) {
        yError() << "Cannot open predicted position port";
        this->close();
        return false;
    }
    predictedPositionPort->setWriteOnly();

    predictedAngularVelocityPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedAngularVelocityPort->open("/oculus/predicted/headpose/angularVelocity:o")) {
        yError() << "Cannot open predicted angular velocity port";
        this->close();
        return false;
    }
    predictedAngularVelocityPort->setWriteOnly();

    predictedLinearVelocityPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedLinearVelocityPort->open("/oculus/predicted/headpose/linearVelocity:o")) {
        yError() << "Cannot open predicted linear velocity port";
        this->close();
        return false;
    }
    predictedLinearVelocityPort->setWriteOnly();

    predictedAngularAccelerationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedAngularAccelerationPort->open("/oculus/predicted/headpose/angularAcceleration:o")) {
        yError() << "Cannot open predicted angular acceleration port";
        this->close();
        return false;
    }
    predictedAngularAccelerationPort->setWriteOnly();

    predictedLinearAccelerationPort = new yarp::os::BufferedPort<yarp::os::Bottle>;
    if (!predictedLinearAccelerationPort->open("/oculus/predicted/headpose/linearAcceleration:o")) {
        yError() << "Cannot open predicted linear acceleration port";
        this->close();
        return false;
    }
    predictedLinearAccelerationPort->setWriteOnly();

    for (int eye = 0; eye < ovrEye_Count; ++eye) {
        displayPorts[eye] = new InputCallback(eye);
        if (!displayPorts[eye]->open(eye == ovrEye_Left ? "/oculus/display/left:i" : "/oculus/display/right:i")) {
            yError() << "Cannot open " << (eye == ovrEye_Left ? "left" : "right") << "display port";
            this->close();
            return false;
        }
        displayPorts[eye]->setReadOnly();
    }

    texWidth  = cfg.check("w",    yarp::os::Value(640), "Texture width (usually same as camera width)").asInt();
    texHeight = cfg.check("h",    yarp::os::Value(480), "Texture height (usually same as camera height)").asInt();

    // TODO accept different fov for right and left eye?
    double hfov   = cfg.check("hfov", yarp::os::Value(105.),  "Camera horizontal field of view").asDouble();
    camHFOV[0] = hfov;
    camHFOV[1] = hfov;

    if (cfg.check("flipinput", "[F] Enable input flipping")) {
        flipInputEnabled = true;
    }

    if (cfg.check("no-imagepose", "[I] Disable image pose")) {
        imagePoseEnabled = false;
    }

    if (cfg.check("userpose", "[U] Use user pose instead of camera pose")) {
        userPoseEnabled = true;
    }

    if (cfg.check("no-logo", "[L] Disable logo")) {
        logoEnabled = false;
    }

    if (cfg.check("no-crosshairs", "[C] Disable crosshairs")) {
        crosshairsEnabled = false;
    }

    if (cfg.check("no-battery", "[C] Disable battery")) {
        batteryEnabled = false;
    }

    prediction = cfg.check("prediction", yarp::os::Value(0.01), "Prediction [sec]").asDouble();

    displayPorts[0]->rollOffset  = static_cast<float>(cfg.check("left-roll-offset",   yarp::os::Value(0.0), "[LEFT_SHIFT+PAGE_UP][LEFT_SHIFT+PAGE_DOWN] Left eye roll offset").asDouble());
    displayPorts[0]->pitchOffset = static_cast<float>(cfg.check("left-pitch-offset",  yarp::os::Value(0.0), "[LEFT_SHIFT+UP_ARROW][LEFT_SHIFT+DOWN_ARROW] Left eye pitch offset").asDouble());
    displayPorts[0]->yawOffset   = static_cast<float>(cfg.check("left-yaw-offset",    yarp::os::Value(0.0), "[LEFT_SHIFT+LEFT_ARROW][LEFT_SHIFT+RIGHT_ARROW] Left eye yaw offset").asDouble());
    displayPorts[1]->rollOffset  = static_cast<float>(cfg.check("right-roll-offset",  yarp::os::Value(0.0), "[RIGHT_SHIFT+PAGE_UP][RIGHT_SHIFT+PAGE_DOWN] Right eye roll offset").asDouble());
    displayPorts[1]->pitchOffset = static_cast<float>(cfg.check("right-pitch-offset", yarp::os::Value(0.0), "[RIGHT_SHIFT+UP_ARROW][RIGHT_SHIFT+DOWN_ARROW] Right eye pitch offset").asDouble());
    displayPorts[1]->yawOffset   = static_cast<float>(cfg.check("right-yaw-offset",   yarp::os::Value(0.0), "[RIGHT_SHIFT+LEFT_ARROW][RIGHT_SHIFT+RIGHT_ARROW] Right eye yaw offset").asDouble());

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
    ovrInitParams initParams = { ovrInit_RequestVersion, OVR_MINOR_VERSION, ovrDebugCallback, reinterpret_cast<uintptr_t>(this), 0 };
    ovrResult r = ovr_Initialize(&initParams);
//    VALIDATE(OVR_SUCCESS(r), "Failed to initialize libOVR.");
    if (!OVR_SUCCESS(r)) {
        yError() << "Failed to initialize libOVR.";
    }
    //Initialise rift
//    if (!ovr_Initialize(&params)) {
//        yError() << "Unable to initialize LibOVR. LibOVRRT not found?";
//        this->close();
//        return false;
//    }

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
    if( !glfwInit() ) {
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
    if(err != GLEW_OK) {
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

    if (orientationPort) {
        orientationPort->interrupt();
        orientationPort->close();
        delete orientationPort;
        orientationPort = nullptr;
    }
    if (positionPort) {
        positionPort->interrupt();
        positionPort->close();
        delete positionPort;
        positionPort = nullptr;
    }
    if (angularVelocityPort) {
        angularVelocityPort->interrupt();
        angularVelocityPort->close();
        delete angularVelocityPort;
        angularVelocityPort = nullptr;
    }
    if (linearVelocityPort) {
        linearVelocityPort->interrupt();
        linearVelocityPort->close();
        delete linearVelocityPort;
        linearVelocityPort = nullptr;
    }
    if (angularAccelerationPort) {
        angularAccelerationPort->interrupt();
        angularAccelerationPort->close();
        delete angularAccelerationPort;
        angularAccelerationPort = nullptr;
    }
    if (linearAccelerationPort) {
        linearAccelerationPort->interrupt();
        linearAccelerationPort->close();
        delete linearAccelerationPort;
        linearAccelerationPort = nullptr;
    }


    if (predictedOrientationPort) {
        predictedOrientationPort->interrupt();
        predictedOrientationPort->close();
        delete predictedOrientationPort;
        predictedOrientationPort = nullptr;
    }
    if (predictedPositionPort) {
        predictedPositionPort->interrupt();
        predictedPositionPort->close();
        delete predictedPositionPort;
        predictedPositionPort = nullptr;
    }
    if (predictedAngularVelocityPort) {
        predictedAngularVelocityPort->interrupt();
        predictedAngularVelocityPort->close();
        delete predictedAngularVelocityPort;
        predictedAngularVelocityPort = nullptr;
    }
    if (predictedLinearVelocityPort) {
        predictedLinearVelocityPort->interrupt();
        predictedLinearVelocityPort->close();
        delete predictedLinearVelocityPort;
        predictedLinearVelocityPort = nullptr;
    }
    if (predictedAngularAccelerationPort) {
        predictedAngularAccelerationPort->interrupt();
        predictedAngularAccelerationPort->close();
        delete predictedAngularAccelerationPort;
        predictedAngularAccelerationPort = nullptr;
    }
    if (predictedLinearAccelerationPort) {
        predictedLinearAccelerationPort->interrupt();
        predictedLinearAccelerationPort->close();
        delete predictedLinearAccelerationPort;
        predictedLinearAccelerationPort = nullptr;
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

    const double delay = 5.0;
    yDebug("Thread ran %d times, est period %lf[ms], used %lf[ms]",
           getIterations(),
           getEstPeriod(),
           getEstUsed());
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

    yarp::os::Time::delay(delay);
    return !closed;
}

bool yarp::dev::OVRHeadset::stopService()
{
    yTrace();
    return this->close();
}

// static void debugPose(const ovrPosef headpose,  ovrPosef EyeRenderPose[2])
// {
//     float head[3];
//     float eye0[3];
//     float eye1[3];
//
//     OVR::Quatf horientation = headpose.Orientation;
//     OVR::Quatf e0orientation = EyeRenderPose[0].Orientation;
//     OVR::Quatf e1orientation = EyeRenderPose[1].Orientation;
//
//     horientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&head[0], &head[1], &head[2]);
//     e0orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&eye0[0], &eye0[1], &eye0[2]);
//     e1orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&eye1[0], &eye1[1], &eye1[2]);
//
//     double iod0 = sqrt(pow(2, EyeRenderPose[0].Position.x - headpose.Position.x) +
//                        pow(2, EyeRenderPose[0].Position.y - headpose.Position.y) +
//                        pow(2, EyeRenderPose[0].Position.z - headpose.Position.z));
//     double iod1 = sqrt(pow(2, EyeRenderPose[1].Position.x - headpose.Position.x) +
//                        pow(2, EyeRenderPose[1].Position.y - headpose.Position.y) +
//                        pow(2, EyeRenderPose[1].Position.z - headpose.Position.z));
//
//     yDebug("head    yaw: %f, pitch: %f, roll: %f, x: %f, y: %f, z: %f\n", head[0], head[1], head[2], headpose.Position.x,  headpose.Position.y, headpose.Position.z);
//     yDebug("eye0         %f,        %f,       %f     %f     %f     %f\n", eye0[0], eye0[1], eye0[2], EyeRenderPose[0].Position.x,  EyeRenderPose[0].Position.y, EyeRenderPose[0].Position.z);
//     yDebug("eye1         %f,        %f,       %f     %f     %f     %f     %f\n\n", eye1[0], eye1[1], eye1[2], EyeRenderPose[1].Position.x,  EyeRenderPose[1].Position.y, EyeRenderPose[1].Position.z, iod1 - iod0);
// }


// static void debugPose(const ovrPosef pose, const char* name = "")
// {
//     float roll, pitch, yaw;
//     OVR::Quatf orientation = pose.Orientation;
//     orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&roll, &pitch, &yaw);
//     yDebug("%s    yaw: %f, pitch: %f, roll: %f, x: %f, y: %f, z: %f\n",
//            name,
//            roll,
//            pitch,
//            yaw,
//            pose.Position.x,
//            pose.Position.y,
//            pose.Position.z);
// }


void yarp::dev::OVRHeadset::run()
{
    if (glfwWindowShouldClose(window)) {
        close();
        return;
    }

    ovrSessionStatus sessionStatus;
    ovr_GetSessionStatus(session, &sessionStatus);
    if (sessionStatus.ShouldQuit) {
        close();
        return;
    }
    if (sessionStatus.ShouldRecenter) {
        ovr_RecenterTrackingOrigin(session);
    }

    // Check window events;
    glfwPollEvents();

    if (!sessionStatus.IsVisible) {
        return;
    }

    // Begin frame
    ++distortionFrameIndex;
//    double frameTiming = ovr_GetPredictedDisplayTime(session, distortionFrameIndex);

    // Query the HMD for the current tracking state.
    ovrTrackingState ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds(), false);
    ovrPoseStatef headpose = ts.HeadPose;
    yarp::os::Stamp stamp(distortionFrameIndex, ts.HeadPose.TimeInSeconds);

    //Get eye poses, feeding in correct IPD offset
    ovrVector3f ViewOffset[2] = {EyeRenderDesc[0].HmdToEyeOffset,EyeRenderDesc[1].HmdToEyeOffset};
    ovrPosef EyeRenderPose[2];
    ovr_CalcEyePoses(headpose.ThePose, ViewOffset, EyeRenderPose);

    // Query the HMD for the predicted state
    ovrTrackingState predicted_ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds() + prediction, false);
    ovrPoseStatef predicted_headpose = predicted_ts.HeadPose;
    yarp::os::Stamp predicted_stamp(distortionFrameIndex, predicted_ts.HeadPose.TimeInSeconds);

    // Read orientation and write it on the port
    if (ts.StatusFlags & ovrStatus_OrientationTracked) {

        if (orientationPort->getOutputCount() > 0) {
            OVR::Quatf orientation = headpose.ThePose.Orientation;
            float yaw, pitch, roll;
            orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
            yarp::os::Bottle& output_orientation = orientationPort->prepare();
            output_orientation.clear();
            output_orientation.addDouble(OVR::RadToDegree(pitch));
            output_orientation.addDouble(OVR::RadToDegree(-roll));
            output_orientation.addDouble(OVR::RadToDegree(yaw));
            orientationPort->setEnvelope(stamp);
            orientationPort->write();
        }

        if (angularVelocityPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_angularVelocity = angularVelocityPort->prepare();
            output_angularVelocity.addDouble(OVR::RadToDegree(headpose.AngularVelocity.x));
            output_angularVelocity.addDouble(OVR::RadToDegree(headpose.AngularVelocity.y));
            output_angularVelocity.addDouble(OVR::RadToDegree(headpose.AngularVelocity.z));
            angularVelocityPort->setEnvelope(stamp);
            angularVelocityPort->write();
        }

        if (angularAccelerationPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_angularAcceleration = angularAccelerationPort->prepare();
            output_angularAcceleration.addDouble(OVR::RadToDegree(headpose.AngularAcceleration.x));
            output_angularAcceleration.addDouble(OVR::RadToDegree(headpose.AngularAcceleration.y));
            output_angularAcceleration.addDouble(OVR::RadToDegree(headpose.AngularAcceleration.z));
            angularAccelerationPort->setEnvelope(stamp);
            angularAccelerationPort->write();
        }

    } else {
        // Do not warn more than once every 5 seconds
        static double lastOrientWarnTime = 0;
        double now = yarp::os::Time::now();
        if(now >= lastOrientWarnTime + 5) {
            yDebug() << "Orientation not tracked";
            lastOrientWarnTime = now;
        }
    }

    // Read position and write it on the port
    if (ts.StatusFlags & ovrStatus_PositionTracked) {

        if (positionPort->getOutputCount() > 0) {
            OVR::Vector3f position = headpose.ThePose.Position;
            yarp::os::Bottle& output_position = positionPort->prepare();
            output_position.clear();
            output_position.addDouble(position[0]);
            output_position.addDouble(position[1]);
            output_position.addDouble(position[2]);
            positionPort->setEnvelope(stamp);
            positionPort->write();
        }

        if (linearVelocityPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_linearVelocity = linearVelocityPort->prepare();
            output_linearVelocity.addDouble(headpose.LinearVelocity.x);
            output_linearVelocity.addDouble(headpose.LinearVelocity.y);
            output_linearVelocity.addDouble(headpose.LinearVelocity.z);
            linearVelocityPort->setEnvelope(stamp);
            linearVelocityPort->write();
        }

        if (linearAccelerationPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_linearAcceleration = linearAccelerationPort->prepare();
            output_linearAcceleration.addDouble(headpose.LinearAcceleration.x);
            output_linearAcceleration.addDouble(headpose.LinearAcceleration.y);
            output_linearAcceleration.addDouble(headpose.LinearAcceleration.z);
            linearAccelerationPort->setEnvelope(stamp);
            linearAccelerationPort->write();
        }

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPosWarnTime = 0;
        double now = yarp::os::Time::now();
        if(now >= lastPosWarnTime + 5) {
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
            output_orientation.addDouble(OVR::RadToDegree(pitch));
            output_orientation.addDouble(OVR::RadToDegree(-roll));
            output_orientation.addDouble(OVR::RadToDegree(yaw));
            predictedOrientationPort->setEnvelope(predicted_stamp);
            predictedOrientationPort->write();
        }

        if (predictedAngularVelocityPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_angularVelocity = predictedAngularVelocityPort->prepare();
            output_angularVelocity.addDouble(OVR::RadToDegree(predicted_headpose.AngularVelocity.x));
            output_angularVelocity.addDouble(OVR::RadToDegree(predicted_headpose.AngularVelocity.y));
            output_angularVelocity.addDouble(OVR::RadToDegree(predicted_headpose.AngularVelocity.z));
            predictedAngularVelocityPort->setEnvelope(predicted_stamp);
            predictedAngularVelocityPort->write();
        }

        if (predictedAngularAccelerationPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_angularAcceleration = predictedAngularAccelerationPort->prepare();
            output_angularAcceleration.addDouble(OVR::RadToDegree(predicted_headpose.AngularAcceleration.x));
            output_angularAcceleration.addDouble(OVR::RadToDegree(predicted_headpose.AngularAcceleration.y));
            output_angularAcceleration.addDouble(OVR::RadToDegree(predicted_headpose.AngularAcceleration.z));
            predictedAngularAccelerationPort->setEnvelope(predicted_stamp);
            predictedAngularAccelerationPort->write();
        }

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPredOrientWarnTime = 0;
        double now = yarp::os::Time::now();
        if(now >= lastPredOrientWarnTime + 5) {
            yDebug() << "Predicted orientation not tracked";
            lastPredOrientWarnTime = now;
        }
    }

    // Read predicted position and write it on the port
    if (predicted_ts.StatusFlags & ovrStatus_PositionTracked) {

        if (predictedPositionPort->getOutputCount() > 0) {
            OVR::Vector3f position = predicted_headpose.ThePose.Position;
            yarp::os::Bottle& output_position = predictedPositionPort->prepare();
            output_position.clear();
            output_position.addDouble(position[0]);
            output_position.addDouble(position[1]);
            output_position.addDouble(position[2]);
            predictedPositionPort->setEnvelope(predicted_stamp);
            predictedPositionPort->write();
        }

        if (predictedLinearVelocityPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_linearVelocity = predictedLinearVelocityPort->prepare();
            output_linearVelocity.addDouble(predicted_headpose.LinearVelocity.x);
            output_linearVelocity.addDouble(predicted_headpose.LinearVelocity.y);
            output_linearVelocity.addDouble(predicted_headpose.LinearVelocity.z);
            predictedLinearVelocityPort->setEnvelope(predicted_stamp);
            predictedLinearVelocityPort->write();
        }

        if (predictedLinearAccelerationPort->getOutputCount() > 0) {
            yarp::os::Bottle& output_linearAcceleration = predictedLinearAccelerationPort->prepare();
            output_linearAcceleration.addDouble(predicted_headpose.LinearAcceleration.x);
            output_linearAcceleration.addDouble(predicted_headpose.LinearAcceleration.y);
            output_linearAcceleration.addDouble(predicted_headpose.LinearAcceleration.z);
            predictedLinearAccelerationPort->setEnvelope(predicted_stamp);
            predictedLinearAccelerationPort->write();
        }

    } else {
        // Do not warn more than once every 5 seconds
        static double lastPredPosWarnTime = 0;
        double now = yarp::os::Time::now();
        if(now >= lastPredPosWarnTime + 5) {
            yDebug() << "Position not tracked";
            lastPredPosWarnTime = now;
        }
    }


    if(displayPorts[0]->eyeRenderTexture && displayPorts[1]->eyeRenderTexture) {
        // Do distortion rendering, Present and flush/sync

        //static double ttt =yarp::os::Time::now();
        //yDebug () << yarp::os::Time::now() - ttt;
        //ttt = yarp::os::Time::now();
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

//          for (int eye = 0; eye < ovrEye_Count; ++eye) {
//              debugPose(displayPorts[eye]->eyeRenderTexture->eyePose, (eye == ovrEye_Left ? "camera left" : "camera right"));
//              debugPose(EyeRenderPose[eye], (eye == ovrEye_Left ? "render left" : "render right"));
//          }

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

        ovrPoseStatef lhandpose = ts.HandPoses[0];

        layerList.push_back(&eyeLayer.Header);

        ovrLayerQuad logoLayer;
        if (logoEnabled) {
            logoLayer.Header.Type = ovrLayerType_Quad;
            logoLayer.Header.Flags = ovrLayerFlag_HeadLocked;
            logoLayer.ColorTexture = textureLogo->textureSwapChain;

            // 50cm in front and 20cm down from the player's nose,
            // fixed relative to their torso.textureLogo
            logoLayer.QuadPoseCenter.Position.x = 0.20f;
            logoLayer.QuadPoseCenter.Position.y = -0.20f;
            logoLayer.QuadPoseCenter.Position.z = -0.50f;
            logoLayer.QuadPoseCenter.Orientation.x = 0;
            logoLayer.QuadPoseCenter.Orientation.y = 0;
            logoLayer.QuadPoseCenter.Orientation.z = 0;
            logoLayer.QuadPoseCenter.Orientation.w = 1;

            // Logo is 5cm wide, 5cm tall.
            logoLayer.QuadSize.x = 0.05f;
            logoLayer.QuadSize.y = 0.05f;
            // Display all of the HUD texture.
            logoLayer.Viewport = OVR::Recti(0, 0, textureLogo->width, textureLogo->height);
            layerList.push_back(&logoLayer.Header);
        }

        ovrLayerQuad crosshairsLayer;
        if (crosshairsEnabled) {
            crosshairsLayer.Header.Type = ovrLayerType_Quad;
            crosshairsLayer.Header.Flags = ovrLayerFlag_HeadLocked;
            crosshairsLayer.ColorTexture = textureCrosshairs->textureSwapChain;

            // 50cm in front and 20cm down from the player's nose,
            // fixed relative to their torso.textureLogo
            crosshairsLayer.QuadPoseCenter.Position.x = 0.0f;
            crosshairsLayer.QuadPoseCenter.Position.y = 0.0f;
            crosshairsLayer.QuadPoseCenter.Position.z = -5.0f;
            crosshairsLayer.QuadPoseCenter.Orientation.x = 0;
            crosshairsLayer.QuadPoseCenter.Orientation.y = 0;
            crosshairsLayer.QuadPoseCenter.Orientation.z = 0;
            crosshairsLayer.QuadPoseCenter.Orientation.w = 1;

            // HUD is 8cm wide, 8cm tall.
            crosshairsLayer.QuadSize.x = 0.08f;
            crosshairsLayer.QuadSize.y = 0.08f;
            // Display all of the HUD texture.
            crosshairsLayer.Viewport = OVR::Recti(0, 0, textureCrosshairs->width, textureCrosshairs->height);
            layerList.push_back(&crosshairsLayer.Header);
        }

        ovrLayerQuad batteryLayer;
        if (batteryEnabled) {
            batteryLayer.Header.Type = ovrLayerType_Quad;
            batteryLayer.Header.Flags = ovrLayerFlag_HeadLocked;
            batteryLayer.ColorTexture = textureBattery->currentTexture->textureSwapChain;

            // 50cm in front and 20cm down from the player's nose,
            // fixed relative to their torso.textureLogo
            batteryLayer.QuadPoseCenter.Position.x = 0.25f;
            batteryLayer.QuadPoseCenter.Position.y = 0.25f;
            batteryLayer.QuadPoseCenter.Position.z = -0.50f;
            batteryLayer.QuadPoseCenter.Orientation.x = 0;
            batteryLayer.QuadPoseCenter.Orientation.y = 0;
            batteryLayer.QuadPoseCenter.Orientation.z = 0;
            batteryLayer.QuadPoseCenter.Orientation.w = 1;

            // Logo is 5cm wide, 5cm tall.
            batteryLayer.QuadSize.x = 0.05f;
            batteryLayer.QuadSize.y = 0.05f;
            // Display all of the HUD texture.
            batteryLayer.Viewport = OVR::Recti(0, 0, textureBattery->currentTexture->width, textureBattery->currentTexture->height);
            layerList.push_back(&batteryLayer.Header);
        }

        ovrLayerHeader** layers = new ovrLayerHeader*[layerList.size()];
        std::copy(layerList.begin(), layerList.end(), layers);
        ovrResult result = ovr_SubmitFrame(session, distortionFrameIndex, nullptr, layers, layerList.size());
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
        double now = yarp::os::Time::now();
        if(now >= lastImgWarnTime + 5) {
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
        break;
    case GLFW_KEY_U:
        userPoseEnabled = !userPoseEnabled;
        yDebug() << "User pose" << (userPoseEnabled ? "ON" : "OFF");
        break;
    case GLFW_KEY_L:
        logoEnabled = !logoEnabled;
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF");
        break;
    case GLFW_KEY_C:
        crosshairsEnabled = !crosshairsEnabled;
        yDebug() << "Overlays:" <<
            "Logo" << (logoEnabled ? "ON" : "OFF") <<
            "Crosshairs" << (crosshairsEnabled ? "ON" : "OFF") <<
            "Battery" << (batteryEnabled ? "ON" : "OFF");
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
            "Battery" << (batteryEnabled ? "ON" : "OFF");
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
            displayPorts[0]->pitchOffset += rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye pitch offset =" << displayPorts[0]->pitchOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->pitchOffset += leftCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye pitch offset =" << displayPorts[1]->pitchOffset;
        }
        break;
    case GLFW_KEY_DOWN:
        if (!rightShiftPressed) {
            displayPorts[0]->pitchOffset -= rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye pitch offset =" << displayPorts[0]->pitchOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->pitchOffset -= leftCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye pitch offset =" << displayPorts[1]->pitchOffset;
        }
        break;
    case GLFW_KEY_LEFT:
        if (!rightShiftPressed) {
            displayPorts[0]->yawOffset += rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye yaw offset =" << displayPorts[0]->yawOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->yawOffset += leftCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye yaw offset =" << displayPorts[1]->yawOffset;
        }
        break;
    case GLFW_KEY_RIGHT:
        if (!rightShiftPressed) {
            displayPorts[0]->yawOffset -= rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye yaw offset =" << displayPorts[0]->yawOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->yawOffset -= leftCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye yaw offset =" << displayPorts[1]->yawOffset;
        }
        break;
    case GLFW_KEY_PAGE_UP:
        if (!rightShiftPressed) {
            displayPorts[0]->rollOffset += rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye roll offset =" << displayPorts[0]->rollOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->rollOffset += leftCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Right eye roll offset =" << displayPorts[1]->rollOffset;
        }
        break;
    case GLFW_KEY_PAGE_DOWN:
        if (!rightShiftPressed) {
            displayPorts[0]->rollOffset -= rightCtrlPressed ? 0.05f : 0.0025f;
            yDebug() << "Left eye roll offset =" << displayPorts[0]->rollOffset;
        }
        if (!leftShiftPressed) {
            displayPorts[1]->rollOffset -= leftCtrlPressed ? 0.05f : 0.0025f;
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
