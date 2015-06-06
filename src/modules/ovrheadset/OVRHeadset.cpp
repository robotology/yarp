/*
 * Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include "OVRHeadset.h"
#include "InputCallback.h"
#include "TextureBuffer.h"

#include <yarp/os/BufferedPort.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>
#include <yarp/os/LogStream.h>

#include <math.h>

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

#include <OVR.h>
#include <OVR_System.h>
#include <OVR_CAPI_GL.h>




#define checkGlErrorMacro yarp::dev::OVRHeadset::checkGlError(__FILE__, __LINE__)


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

yarp::dev::OVRHeadset::OVRHeadset() :
        yarp::dev::DeviceDriver(),
        yarp::os::RateThread(13), // ~75 fps
        orientationPort(NULL),
        positionPort(NULL),
        window(NULL),
        closed(false),
        distortionFrameIndex(0),
        multiSampleEnabled(false),
        overdriveEnabled(true),
        hqDistortionEnabled(true),
        flipInputEnabled(true),
        timeWarpEnabled(true),
        imagePoseEnabled(true)
{
    yTrace();

#if !XXX_DUAL
    displayPorts[0] = NULL;
    displayPorts[1] = NULL;
    displayPortCallbacks[0] = NULL;
    displayPortCallbacks[1] = NULL;
#else
    displayPort = NULL;
    displayPortCallback = NULL;
#endif

    yarp::os::Time::turboBoost();
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

#if !XXX_DUAL
    for (int i = 0; i < 2; ++i) {
        displayPorts[i] = new yarp::os::BufferedPort<ImageType>;
        if (!displayPorts[i]->open(i == 0 ? "/oculus/display/left:i" : "/oculus/display/right:i")) {
            yError() << "Cannot open " << (i == 0 ? "left" : "right") << "display port";
            this->close();
            return false;
        }
        displayPorts[i]->setReadOnly();
//        displayPorts[i]->setStrict();

        displayPortCallbacks[i] = new InputCallback(i);
    }
#else
    displayPort = new yarp::os::BufferedPort<ImageType>;
    if (!displayPort->open("/oculus/display/dual:i")) {
        yError() << "Cannot open dual display port";
        this->close();
        return false;
    }
    displayPort->setReadOnly();

    displayPortCallback = new InputCallback;
#endif



    texWidth  = cfg.check("w",    yarp::os::Value(640), "Texture width (usually same as camera width)").asInt();
    texHeight = cfg.check("h",    yarp::os::Value(480), "Texture height (usually same as camera height)").asInt();

    // TODO accept different fov for right and left eye?
    double hfov   = cfg.check("hfov", yarp::os::Value(105.),  "Camera horizontal field of view").asDouble();
    camHFOV[0] = hfov;
    camHFOV[1] = hfov;

    if (cfg.check("multisample", "[M] Enable multisample")) {
        multiSampleEnabled = true;
    }

    if (cfg.check("no-overdrive", "[O] Disable overdrive")) {
        overdriveEnabled = false;
    }

    if (cfg.check("no-hqdistortion", "[H] Disable high quality distortion")) {
        hqDistortionEnabled = false;
    }

    if (cfg.check("no-flipinput", "[F] Disable input flipping")) {
        flipInputEnabled = false;
    }

    if (cfg.check("no-timewarp", "[T] Disable timewarp")) {
        timeWarpEnabled = false;
    }

    if (cfg.check("no-imagepose", "[I] Disable image pose")) {
        imagePoseEnabled = false;
    }


//    userHeight = cfg.check("userHeight", yarp::os::Value(0.),  "User height").asDouble();


    // Start the thread
    if (!this->start()) {
        yError() << "thread start failed, aborting.";
        this->close();
        return false;
    }

    // Enable display port callbacks
    for (int i=0; i<2; i++) {
#if !XXX_DUAL
        displayPorts[i]->useCallback(*(displayPortCallbacks[i]));
#else
        displayPort->useCallback(*(displayPortCallback));
#endif
    }


#if !XXX_DUAL
//    yarp::os::Network::connect("/icubSim/cam/left", "/oculus/display/left:i", "mjpeg");
//    yarp::os::Network::connect("/icubSim/cam/right", "/oculus/display/right:i", "mjpeg");
//    yarp::os::Network::connect("/oculus/headpose/orientation:o", "/directPositionControl/icubSim/head/command:i", "udp");

//    yarp::os::Network::connect("/icub/camcalib/left/out", "/oculus/display/left:i", "mjpeg");
//    yarp::os::Network::connect("/icub/camcalib/right/out", "/oculus/display/right:i", "mjpeg");

//    yarp::os::Network::connect("/icub/cam/left", "/oculus/display/left:i", "mjpeg");
//    yarp::os::Network::connect("/icub/cam/right", "/oculus/display/right:i", "mjpeg");

    yarp::os::Network::connect("/gazeboCamera/left", "/oculus/display/left:i", "udp");
    yarp::os::Network::connect("/gazeboCamera/right", "/oculus/display/right:i", "udp");
    yarp::os::Network::connect("/oculus/headpose/orientation:o", "/directPositionControl/icubGazeboSim/head/command:i", "tcp");

//    yarp::os::Network::connect("/grabber", "/oculus/display/left:i", "udp");
//    yarp::os::Network::connect("/grabber", "/oculus/display/right:i", "udp");
#else
    yarp::os::Network::connect("/camCalib/out", "/oculus/display/dual:i", "mjpeg");
#endif

    return true;
}

bool yarp::dev::OVRHeadset::threadInit()
{
    yTrace();

    OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));

    ovrInitParams params;
    params.Flags = 0;
    params.RequestedMinorVersion = 0;
    params.LogCallback = ovrDebugCallback;
    params.ConnectionTimeoutMS = 0;

    //Initialise rift
    if (!ovr_Initialize(&params)) {
        yError() << "Unable to initialize LibOVR.";
        this->close();
        return false;
    }

    // Detect and initialize Oculus Rift
    hmd = ovrHmd_Create(0);
    if (hmd) {
        yInfo() << "Oculus Rift FOUND.";
    } else {
        yWarning() << "Oculus Rift NOT FOUND. Using debug device.";
        hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
    }

    // If still not existing, we failed initializing it.
    if (!hmd) {
        yError() << "Oculus Rift not detected.";
        this->close();
        return false;
    }

    if (hmd->ProductName[0] == '\0') {
        yWarning() << "Rift detected, display not enabled.";
    }

    DebugHmd(hmd);


    // Initialize the GLFW system for creating and positioning windows
    // GLFW must be initialized after LibOVR
    // see http://www.glfw.org/docs/latest/rift.html
    if( !glfwInit() ) {
        yError() << "Failed to initialize GLFW";
        this->close();
        return false;
    }
    glfwSetErrorCallback(glfwErrorCallback);

//    bool windowed = (hmd->HmdCaps & ovrHmdCap_ExtendDesktop) ? false : true;
    OVR::Sizei windowSize = hmd->Resolution;

    if (!createWindow(windowSize.w/2, windowSize.h/2, hmd->WindowsPos.x+30, hmd->WindowsPos.y+30)) {
        yError() << "Failed to create window";
        this->close();
        return false;
    }


    // Initialize the GLEW OpenGL 3.x bindings
    // GLEW must be initialized after creating the window
    glewExperimental=GL_TRUE;
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


    config.OGL.Header.API              = ovrRenderAPI_OpenGL;
    config.OGL.Header.BackBufferSize.w = fbwidth;
    config.OGL.Header.BackBufferSize.h = fbheight;
    config.OGL.Header.Multisample      = (multiSampleEnabled ? 1 : 0);
#if defined(_WIN32)
    config.OGL.Window = glfwGetWin32Window(window);
    config.OGL.DC     = GetDC(glfwGetWin32Window(window));
#elif defined(__APPLE__)
#elif defined(__linux__)
    config.OGL.Disp = glfwGetX11Display();
#endif

    for (int i = 0; i < 2; ++i) {
        camWidth[i] = texWidth;
        camHeight[i] = texHeight;
    }
    reconfigureFOV();

    reconfigureRendering();

    ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence |
                               ovrHmdCap_DynamicPrediction);

#if defined(_WIN32)
    ovrHmd_AttachToWindow(hmd, glfwGetWin32Window(window), NULL, NULL);
#endif

    ovrHmd_DismissHSWDisplay(hmd);


    for (int i=0; i<2; i++)
    {
//        OVR::Sizei idealTextureSize = ovrHmd_GetFovTextureSize(hmd, (ovrEyeType)i, hmd->DefaultEyeFov[i], 1);
#if !XXX_DUAL
        displayPortCallbacks[i]->eyeRenderTexture = new TextureBuffer(texWidth, texHeight, i);
//        displayPortCallbacks[i]->eyeRenderTexture = new TextureBuffer(idealTextureSize.w, idealTextureSize.h, i);
#else
        displayPortCallback->eyeRenderTextures[i] = new TextureBuffer(texWidth, texHeight, i);
//        displayPortCallback->eyeRenderTextures[i] = new TextureBuffer(idealTextureSize.w, idealTextureSize.h, i);
        }
#endif
    }

    // Start the sensor which provides the Rift's pose and motion.
    ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
                                  ovrTrackingCap_MagYawCorrection |
                                  ovrTrackingCap_Position , 0);

    // Recenter position
    ovrHmd_RecenterPose(hmd);

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

    // Shut down GLFW
    glfwTerminate();

    // Shut down LibOVR
    if (hmd) {
        ovrHmd_Destroy(hmd);
        hmd = 0;
        ovr_Shutdown();
    }

    if (orientationPort) {
        orientationPort->interrupt();
        orientationPort->close();
        delete orientationPort;
        orientationPort = NULL;
    }
    if (positionPort) {
        positionPort->interrupt();
        positionPort->close();
        delete positionPort;
        positionPort = NULL;
    }

#if !XXX_DUAL
    for (int i = 0; i < 2; ++i) {
        if (displayPorts[i]) {
            displayPorts[i]->disableCallback();
            displayPorts[i]->interrupt();
            displayPorts[i]->close();
            delete displayPorts[i];
            displayPorts[i] = NULL;
        }
        if (displayPortCallbacks[i]) {
            delete displayPortCallbacks[i];
            displayPortCallbacks[i] = NULL;
        }
    }
#else
    if (displayPort) {
        displayPort->disableCallback();
        displayPort->interrupt();
        displayPort->close();
        delete displayPort;
        displayPort = NULL;
    }
    if (displayPortCallback) {
        delete displayPortCallback;
        displayPortCallback = NULL;
    }
#endif
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
    yDebug("Thread ran %d times (%f[hz]), est period %lf[ms], used %lf[ms]\n",
           getIterations(),
           getIterations()/delay,
           getEstPeriod(),
           getEstUsed());
    resetStat();
#if !XXX_DUAL
    for (int i = 0; i < 2; ++i) {
        yDebug("%s eye: %d frames missing, %d frames dropped\n",
               (i == 0 ? "Left " : "Right"),
               displayPortCallbacks[i]->eyeRenderTexture->missingFrames,
               displayPortCallbacks[i]->droppedFrames);
        displayPortCallbacks[i]->eyeRenderTexture->missingFrames = 0;
        displayPortCallbacks[i]->droppedFrames = 0;

#else
    yDebug("Missing frames: %d (left), %d (right)\n",
           displayPortCallback->eyeRenderTextures[0]->missingFrames,
           displayPortCallback->eyeRenderTextures[1]->missingFrames,
    yDebug("Dropped frames: %d\n",
           displayPortCallback->droppedFrames);
    displayPortCallback->eyeRenderTextures[0]->missingFrames = 0;
    displayPortCallback->eyeRenderTextures[0]->missingFrames = 0;
    displayPortCallback->droppedFrames = 0;
#endif
    }

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

    // Check window events;
    glfwPollEvents();

    // Begin frame
    ++distortionFrameIndex;
    ovrFrameTiming frameTiming = ovrHmd_BeginFrame(hmd, distortionFrameIndex);

    // Query the HMD for the current tracking state.
    ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());

    //Get eye poses, feeding in correct IPD offset
    ovrVector3f ViewOffset[2] = {EyeRenderDesc[0].HmdToEyeViewOffset,EyeRenderDesc[1].HmdToEyeViewOffset};
    ovrPosef EyeRenderPose[2];
    ovrTrackingState ts_eyes;
    ovrHmd_GetEyePoses(hmd, distortionFrameIndex, ViewOffset, EyeRenderPose, &ts_eyes);

//    debugPose(ts.HeadPose.ThePose,  EyeRenderPose);
//    debugPose(ts_eyes.HeadPose.ThePose,  EyeRenderPose);

    // FIXME ts or ts_eyes?
    ovrPosef headpose = ts.HeadPose.ThePose;

    // Read orientation and write it on the port
    if (ts.StatusFlags & ovrStatus_OrientationTracked) {
        OVR::Quatf orientation = headpose.Orientation;
        float yaw, pitch, roll;
        orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
        yarp::os::Bottle& output_orientation = orientationPort->prepare();
        output_orientation.clear();
        output_orientation.addDouble(OVR::RadToDegree(pitch));
        output_orientation.addDouble(OVR::RadToDegree(-roll));
        output_orientation.addDouble(OVR::RadToDegree(yaw));
        orientationPort->write();
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
        OVR::Vector3f position = headpose.Position;
        yarp::os::Bottle& output_position = positionPort->prepare();
        output_position.clear();
        output_position.addDouble(position[0]);
        output_position.addDouble(position[1]);
        output_position.addDouble(position[2]);
        positionPort->write();
    } else {
        // Do not warn more than once every 5 seconds
        static double lastPosWarnTime = 0;
        double now = yarp::os::Time::now();
        if(now >= lastPosWarnTime + 5) {
            yDebug() << "Position not tracked";
            lastPosWarnTime = now;
        }
    }

#if !XXX_DUAL
    if(displayPortCallbacks[0]->eyeRenderTexture && displayPortCallbacks[1]->eyeRenderTexture) {
#else
    if(displayPortCallback->eyeRenderTextures[0] && displayPortCallback->eyeRenderTextures[1]) {
#endif

        // Do distortion rendering, Present and flush/sync
        ovrGLTexture eyeTex[2];
        for (int i = 0; i<2; ++i) {
            eyeTex[i].OGL.Header.API = ovrRenderAPI_OpenGL;
#if !XXX_DUAL
            eyeTex[i].OGL.Header.TextureSize = OVR::Sizei(displayPortCallbacks[i]->eyeRenderTexture->width, displayPortCallbacks[i]->eyeRenderTexture->height);
            eyeTex[i].OGL.Header.RenderViewport = OVR::Recti(0, 0, displayPortCallbacks[i]->eyeRenderTexture->width, displayPortCallbacks[i]->eyeRenderTexture->height);
            eyeTex[i].OGL.TexId = displayPortCallbacks[i]->eyeRenderTexture->texId;
#else
            eyeTex[i].OGL.Header.TextureSize = OVR::Sizei(displayPortCallback->eyeRenderTextures[i]->width, displayPortCallback->eyeRenderTextures[i]->height);
            eyeTex[i].OGL.Header.RenderViewport = OVR::Recti(0, 0, displayPortCallback->eyeRenderTextures[i]->width, displayPortCallback->eyeRenderTextures[i]->height);
            eyeTex[i].OGL.TexId = displayPortCallback->eyeRenderTextures[i]->texId;
#endif
        }

        // Wait till time-warp point to reduce latency.
        ovr_WaitTillTime(frameTiming.TimewarpPointSeconds - 0.001);

        // Update the textures
        for (int i = 0; i<2; i++) {
#if !XXX_DUAL
            displayPortCallbacks[i]->eyeRenderTexture->update();
#else
            displayPortCallback->eyeRenderTextures[i]->update();
#endif
        }


        if (imagePoseEnabled) {
            // Use orientation received from the image
            for (int i = 0; i<2; i++) {
                EyeRenderPose[i].Orientation = displayPortCallbacks[i]->eyeRenderTexture->eyePose.Orientation;
            }
        }

//        if (userHeight != 0) {
//            for (int i = 0; i<2; i++) {
//                EyeRenderPose[i].Position.y += static_cast<float>(userHeight);
//            }
//        }


//          for (int i = 0; i<2; i++) {
//              debugPose(displayPortCallbacks[i]->eyeRenderTexture->eyePose, (i==0?"camera left":"camera right"));
//              debugPose(EyeRenderPose[i], (i==0?"render left":"render right"));
//          }

        // If the image size is different from the texture size,
        bool needReconfigureFOV = false;
        for (int i = 0; i<2; i++) {
            if ((displayPortCallbacks[i]->eyeRenderTexture->imageWidth != 0 && displayPortCallbacks[i]->eyeRenderTexture->imageWidth != camWidth[i]) ||
                (displayPortCallbacks[i]->eyeRenderTexture->imageHeight != 0 && displayPortCallbacks[i]->eyeRenderTexture->imageHeight != camHeight[i])) {

                camWidth[i] = displayPortCallbacks[i]->eyeRenderTexture->imageWidth;
                camHeight[i] = displayPortCallbacks[i]->eyeRenderTexture->imageHeight;
                needReconfigureFOV = true;
            }
        }
        if (needReconfigureFOV) {
            reconfigureFOV();
            reconfigureRendering();
        }

        // End frame
        ovrHmd_EndFrame(hmd, EyeRenderPose, &eyeTex[0].Texture);
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


bool yarp::dev::OVRHeadset::createWindow(int w, int h, int x, int y)
{
    yTrace();

    glfwWindowHint(GLFW_DEPTH_BITS, 16);
//    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    window = glfwCreateWindow(w, h, "YARP Oculus", NULL, NULL);
    if (!window) {
        yError() << "Could not create window";
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetWindowPos(window, x, y);
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

    switch (key) {
    case GLFW_KEY_R:
        ovrHmd_RecenterPose(hmd);
        break;
    case GLFW_KEY_T:
        timeWarpEnabled = !timeWarpEnabled;
        reconfigureRendering();
        break;
    case GLFW_KEY_M:
        multiSampleEnabled = !multiSampleEnabled;
        reconfigureRendering();
        break;
    case GLFW_KEY_F:
        flipInputEnabled = !flipInputEnabled;
        reconfigureRendering();
        break;
    case GLFW_KEY_H:
        hqDistortionEnabled = !hqDistortionEnabled;
        reconfigureRendering();
        break;
    case GLFW_KEY_O:
        overdriveEnabled = !overdriveEnabled;
        reconfigureRendering();
        break;
    case GLFW_KEY_I:
        imagePoseEnabled = !imagePoseEnabled;
        break;
    case GLFW_KEY_ESCAPE:
        this->close();
        break;
    default:
        break;
    }
}


void yarp::dev::OVRHeadset::reconfigureRendering()
{
    config.OGL.Header.Multisample = multiSampleEnabled ? 1 : 0;

    unsigned int distortionCaps = 0;
    distortionCaps |= ovrDistortionCap_Vignette;
    if (timeWarpEnabled)      distortionCaps |= ovrDistortionCap_TimeWarp;
    if (overdriveEnabled)     distortionCaps |= ovrDistortionCap_Overdrive;
    if (hqDistortionEnabled)  distortionCaps |= ovrDistortionCap_HqDistortion;
    if (flipInputEnabled)     distortionCaps |= ovrDistortionCap_FlipInput;

    ovrHmd_ConfigureRendering(hmd, &config.Config, distortionCaps, fov, EyeRenderDesc);
}



void yarp::dev::OVRHeadset::reconfigureFOV()
{
    for (int i = 0; i < 2; ++i) {
        double camHFOV_rad = OVR::DegreeToRad(camHFOV[i]);
        double texCamRatio = static_cast<double>(texWidth)/camWidth[i];
        double texHFOV_rad = 2 * (atan(texCamRatio * tan(camHFOV_rad/2)));

        double aspectRatio = static_cast<double>(texWidth)/texHeight;
        fov[i].UpTan    = static_cast<float>(abs(tan(texHFOV_rad/2)/aspectRatio));
        fov[i].DownTan  = static_cast<float>(abs(tan(texHFOV_rad/2)/aspectRatio));
        fov[i].LeftTan  = static_cast<float>(abs(tan(texHFOV_rad/2)));
        fov[i].RightTan = static_cast<float>(abs(tan(texHFOV_rad/2)));
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
    // FIXME abort?
}


void yarp::dev::OVRHeadset::checkGlError(const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        switch(error) {
        case GL_INVALID_ENUM:
            yError() << "OpenGL Error GL_INVALID_ENUM: GLenum argument out of range";
            break;
        case GL_INVALID_VALUE:
            yError() << "OpenGL Error GL_INVALID_VALUE: Numeric argument out of range";
            break;
        case GL_INVALID_OPERATION:
            yError() << "OpenGL Error GL_INVALID_OPERATION: Operation illegal in current state";
            break;
        case GL_STACK_OVERFLOW:
            yError() << "OpenGL Error GL_STACK_OVERFLOW: Command would cause a stack overflow";
            break;
        case GL_OUT_OF_MEMORY:
            yError() << "OpenGL Error GL_OUT_OF_MEMORY: Not enough memory left to execute command";
            break;
        default:
            yError() << "OpenGL Error " << error;
            break;
        }
    }
    yAssert(error == 0);
}

void yarp::dev::OVRHeadset::ovrDebugCallback(int level, const char* message)
{
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

void yarp::dev::OVRHeadset::DebugHmd(ovrHmd hmd)
{
    yDebug("  * ProductName: %s", hmd->ProductName);
    yDebug("  * Manufacturer: %s", hmd->Manufacturer);
    yDebug("  * VendorId:ProductId: %04X:%04X", hmd->VendorId, hmd->ProductId);
    yDebug("  * SerialNumber: %X", hmd->SerialNumber);
    yDebug("  * Firmware Version: %d.%d", hmd->FirmwareMajor, hmd->FirmwareMinor);
    yDebug("  * Resolution: %dx%d", hmd->Resolution.w, hmd->Resolution.h);
}
