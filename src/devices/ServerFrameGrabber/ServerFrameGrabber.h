/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_SERVERFRAMEGRABBER_H
#define YARP_DEV_SERVERFRAMEGRABBER_H

#include <cstdio>

#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>

// #define YARP_INCLUDING_DEPRECATED_HEADER_YARP_OS_RATETHREAD_H_ON_PURPOSE
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/os/RateThread.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
// #undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_OS_RATETHREAD_H_ON_PURPOSE

#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_DATASOURCE_H_ON_PURPOSE
#include <yarp/dev/DataSource.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_DATASOURCE_H_ON_PURPOSE

#include <yarp/proto/framegrabber/RgbVisualParams_Responder.h>
#include <yarp/proto/framegrabber/FrameGrabberControls_Responder.h>

YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

/**
 * @ingroup dev_impl_wrapper
 *
 * \brief `grabber`: Export a frame grabber to the network.  Provides the
 * IFrameGrabberImage, IFrameGrabberControls, and IAudioGrabberSound
 * interfaces.  The corresponding client is a RemoteFrameGrabber.
 *
 * The network interface is a single Port.
 * Images are streamed out from that Port -- RemoteFrameGrabber
 * uses this stream to provide the IFrameGrabberImage interface.
 * The IFrameGrabberControls functionality is provided via RPC.
 *
 * Here's a command-line example:
 * \verbatim
 [terminal A] yarpdev --device fakeFrameGrabber --width 8 --height 8 --name /grabber --framerate 30
 [terminal B] yarp read /read
 [terminal C] yarp connect /grabber /read
 [terminal C] echo "[get] [gain]" | yarp rpc /grabber
 \endverbatim
 * The yarpdev line starts a FakeFrameGrabber wrapped in a ServerFrameGrabber.
 * Parameters are:
 * --width, --height set the size of the frame in pixels
 * --name portname set the name of the output port
 * --framerate set the frequency (Hz) at which images will be read and broadcast to
 * the network; if the parameter is not set images are provided at the maximum speed
 * supported by the device. Notice that the maximum frame rate is determined by
 * the device.
 *
 * After the "yarp connect" line, image descriptions will show up in
 * terminal B (you could view them with the yarpview application).
 * The "yarp rpc" command should query the gain (0.0 for the test grabber).
 *
 * <TABLE>
 * <TR><TD> Command (text form) </TD><TD> Response </TD><TD> Code equivalent </TD></TR>
 * <TR><TD> [set] [bri] 1.0 </TD><TD> none </TD><TD> setBrightness() </TD></TR>
 * <TR><TD> [set] [gain] 1.0 </TD><TD> none </TD><TD> setGain() </TD></TR>
 * <TR><TD> [set] [shut] 1.0 </TD><TD> none </TD><TD> setShutter() </TD></TR>
 * <TR><TD> [get] [bri] </TD><TD> [is] [bri] 1.0 </TD><TD> getBrightness() </TD></TR>
 * <TR><TD> [get] [gain] </TD><TD> [is] [gain] 1.0 </TD><TD> getGain() </TD></TR>
 * <TR><TD> [get] [shut] </TD><TD> [is] [shut] 1.0 </TD><TD> getShutter() </TD></TR>
 * </TABLE>
 *
 */
class ServerFrameGrabber :
        public yarp::dev::DeprecatedDeviceDriver,
        public yarp::dev::DeviceResponder,
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IAudioVisualGrabber,
        public yarp::dev::IService,
        public yarp::dev::DataSource<yarp::sig::ImageOf<yarp::sig::PixelRgb>>,
        public yarp::dev::DataSource<yarp::sig::ImageOf<yarp::sig::PixelMono>>,
        public yarp::dev::DataSource<yarp::dev::ImageRgbSound>,
        public yarp::dev::DataSource2<yarp::sig::ImageOf<yarp::sig::PixelRgb>,yarp::sig::Sound>
{
private:
    yarp::proto::framegrabber::RgbVisualParams_Responder rgbParser;
    yarp::dev::IRgbVisualParams* rgbVis_p{nullptr};
    yarp::os::Port p;
    yarp::os::Port *p2{nullptr};
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    yarp::os::RateThreadWrapper thread;
YARP_WARNING_POP
    yarp::dev::PolyDriver poly;
    yarp::dev::IFrameGrabberImage *fgImage{nullptr};
    yarp::dev::IFrameGrabberImageRaw *fgImageRaw{nullptr};
    yarp::dev::IAudioGrabberSound *fgSound{nullptr};
    yarp::dev::IAudioVisualGrabber *fgAv{nullptr};
    yarp::dev::IFrameGrabberControls  *fgCtrl{nullptr};
    yarp::dev::IPreciselyTimed *fgTimed{nullptr};
    bool spoke{false}; // location of this variable tickles bug on Solaris/gcc3.2
    bool canDrop{false};
    bool addStamp{false};
    bool active{false};
    bool singleThreaded{false};

    yarp::proto::framegrabber::FrameGrabberControls_Responder ifgCtrl_Responder;

public:
    ServerFrameGrabber() = default;
    ServerFrameGrabber(const ServerFrameGrabber&) = delete;
    ServerFrameGrabber(ServerFrameGrabber&&) = delete;
    ServerFrameGrabber& operator=(const ServerFrameGrabber&) = delete;
    ServerFrameGrabber& operator=(ServerFrameGrabber&&) = delete;
    ~ServerFrameGrabber() override = default;

    bool close() override;
    /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "fakeFrameGrabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;

    //bool read(ConnectionReader& connection) override;

    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply) override;

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    bool getDatum(yarp::dev::ImageRgbSound& imageSound) override;

    virtual bool getDatum(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                          yarp::sig::Sound& sound) override;

    bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image);

    virtual bool getAudioVisual(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                yarp::sig::Sound& sound) override;

    int height() const override;

    int width() const override;

    bool startService() override;

    bool stopService() override;

    bool updateService() override;
};

YARP_WARNING_POP

#endif // YARP_DEV_SERVERFRAMEGRABBER_H
