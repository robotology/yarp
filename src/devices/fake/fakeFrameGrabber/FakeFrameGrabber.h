/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H
#define YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H


#include <yarp/sig/ImageFile.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IPreciselyTimed.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Log.h>
#include <yarp/os/Value.h>
#include <yarp/dev/IRgbVisualParams.h>

#include <cstdio>
#include <random>
#include <condition_variable>
#include "FakeFrameGrabber_ParamsParser.h"

/**
 * @ingroup dev_impl_media dev_impl_fake
 *
 * \brief `fakeFrameGrabber`: A fake camera for testing.
 *
 * Implements the IFrameGrabberImage and IFrameGrabberControls
 * interfaces.
 *
 * Parameters required by this device are shown in class: FakeFrameGrabber_ParamsParser
 */
class FakeFrameGrabber :
#ifndef YARP_NO_DEPRECATED // Since YARP 3.5`
        virtual public yarp::dev::DeviceDriver,
#else
        public yarp::dev::DeviceDriver,
#endif
        public yarp::dev::IFrameGrabberImage,
        public yarp::dev::IFrameGrabberImageRaw,
        public yarp::dev::IFrameGrabberControls,
        public yarp::dev::IFrameGrabberControlsDC1394,
        public yarp::dev::IPreciselyTimed,
        public yarp::dev::IRgbVisualParams,
        public yarp::os::Thread,
        public yarp::os::PortReader,
        public FakeFrameGrabber_ParamsParser
{
public:
    FakeFrameGrabber() = default;
    FakeFrameGrabber(const FakeFrameGrabber&) = delete;
    FakeFrameGrabber(FakeFrameGrabber&&) = delete;
    FakeFrameGrabber& operator=(const FakeFrameGrabber&) = delete;
    FakeFrameGrabber& operator=(FakeFrameGrabber&&) = delete;
    ~FakeFrameGrabber() override = default;

    bool close() override;

    /**
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    bool open(yarp::os::Searchable& config) override;

    // yarp::os::PortReader
    bool read(yarp::os::ConnectionReader& connection) override;

    // yarp::os::Thread
    void run() override;
    void onStop() override;

    void timing();

    //IRgbVisualParams
    int getRgbHeight() override;
    int getRgbWidth() override;
    yarp::dev::ReturnValue getRgbSupportedConfigurations(std::vector<yarp::dev::CameraConfig>& configurations) override;
    yarp::dev::ReturnValue getRgbResolution(int& width, int& height) override;
    yarp::dev::ReturnValue setRgbResolution(int width, int height) override;
    yarp::dev::ReturnValue getRgbFOV(double& horizontalFov, double& verticalFov) override;
    yarp::dev::ReturnValue setRgbFOV(double horizontalFov, double verticalFov) override;
    yarp::dev::ReturnValue getRgbIntrinsicParam(yarp::os::Property& intrinsic) override;
    yarp::dev::ReturnValue getRgbMirroring(bool& mirror) override;
    yarp::dev::ReturnValue setRgbMirroring(bool mirror) override;

    //IFrameGrabberImage
    int height() const override;
    int width() const override;
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;
    yarp::dev::ReturnValue getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;
    yarp::dev::ReturnValue getImageCrop(yarp::dev::cropType_id_t cropType,
                      std::vector<yarp::dev::vertex_t> vertices,
                      yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) override;
    yarp::dev::ReturnValue getImageCrop(yarp::dev::cropType_id_t cropType,
                      std::vector<yarp::dev::vertex_t> vertices,
                      yarp::sig::ImageOf<yarp::sig::PixelMono>& image) override;

    //IPreciselyTimed
    yarp::os::Stamp getLastInputStamp() override;

    //IFrameGrabberControls Interface
    yarp::dev::ReturnValue getCameraDescription(yarp::dev::CameraDescriptor& camera) override;
    yarp::dev::ReturnValue hasFeature(yarp::dev::cameraFeature_id_t feature, bool& hasFeature) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double& value) override;
    yarp::dev::ReturnValue setFeature(yarp::dev::cameraFeature_id_t feature, double value1, double value2) override;
    yarp::dev::ReturnValue getFeature(yarp::dev::cameraFeature_id_t feature, double& value1, double& value2) override;
    yarp::dev::ReturnValue setActive(yarp::dev::cameraFeature_id_t feature, bool onoff) override;
    yarp::dev::ReturnValue getActive(yarp::dev::cameraFeature_id_t feature, bool& isActive) override;
    yarp::dev::ReturnValue hasOnOff(yarp::dev::cameraFeature_id_t feature, bool& HasOnOff) override;
    yarp::dev::ReturnValue hasAuto(yarp::dev::cameraFeature_id_t feature, bool& hasAuto) override;
    yarp::dev::ReturnValue hasManual(yarp::dev::cameraFeature_id_t feature, bool& hasManual) override;
    yarp::dev::ReturnValue setMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode mode) override;
    yarp::dev::ReturnValue getMode(yarp::dev::cameraFeature_id_t feature, yarp::dev::FeatureMode& mode) override;
    yarp::dev::ReturnValue hasOnePush(yarp::dev::cameraFeature_id_t feature, bool& hasOnePush) override;
    yarp::dev::ReturnValue setOnePush(yarp::dev::cameraFeature_id_t feature) override;

    //IFrameGrabberControlsDC1394 Interface
    yarp::dev::ReturnValue getVideoModeMaskDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue getVideoModeDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setVideoModeDC1394(int video_mode) override;
    yarp::dev::ReturnValue getFPSMaskDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue getFPSDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setFPSDC1394(int fps) override;
    yarp::dev::ReturnValue getISOSpeedDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setISOSpeedDC1394(int speed) override;
    yarp::dev::ReturnValue getColorCodingMaskDC1394(unsigned int video_mode,unsigned int& val) override;
    yarp::dev::ReturnValue getColorCodingDC1394(unsigned int& val) override;
    yarp::dev::ReturnValue setColorCodingDC1394(int coding) override;
    yarp::dev::ReturnValue getFormat7MaxWindowDC1394(unsigned int &xdim,unsigned int &ydim,unsigned int &xstep,unsigned int &ystep,unsigned int &xoffstep,unsigned int &yoffstep) override;
    yarp::dev::ReturnValue getFormat7WindowDC1394(unsigned int &xdim,unsigned int &ydim,int &x0,int &y0) override;
    yarp::dev::ReturnValue setFormat7WindowDC1394(unsigned int xdim,unsigned int ydim,int x0,int y0) override;
    yarp::dev::ReturnValue setOperationModeDC1394(bool b1394b) override;
    yarp::dev::ReturnValue getOperationModeDC1394(bool& b1394) override;
    yarp::dev::ReturnValue setTransmissionDC1394(bool bTxON) override;
    yarp::dev::ReturnValue getTransmissionDC1394(bool& bTxON) override;
    yarp::dev::ReturnValue setBroadcastDC1394(bool onoff) override;
    yarp::dev::ReturnValue setDefaultsDC1394() override;
    yarp::dev::ReturnValue setResetDC1394() override;
    yarp::dev::ReturnValue setPowerDC1394(bool onoff) override;
    yarp::dev::ReturnValue setCaptureDC1394(bool bON) override;;
    yarp::dev::ReturnValue getBytesPerPacketDC1394(unsigned int& bpp) override;
    yarp::dev::ReturnValue setBytesPerPacketDC1394(unsigned int bpp) override;

private:
    static constexpr size_t default_w = 128;
    static constexpr size_t default_h = 128;
    static constexpr size_t default_freq = 30;
    static constexpr double default_snr = 0.5;

    yarp::os::Port     m_rpcPort;

    size_t m_ct{0};
    size_t m_bx{0};
    size_t m_by{0};
    unsigned long m_rnd{0};
    double prev{0};
    bool m_have_bg{false};

    yarp::os::Property m_intrinsic;
    std::vector<yarp::dev::CameraConfig> configurations;

    std::random_device rnddev;
    std::default_random_engine randengine{rnddev()};
    std::uniform_int_distribution<int> udist{-1, 1};
    std::uniform_real_distribution<double> ucdist{0.0, 1.0};

    std::mutex curr_buff_mutex;
    size_t curr_buff{1};
    yarp::sig::ImageOf<yarp::sig::PixelRgb> buffs[2];
    bool img_ready[2] {false, false};
    bool img_consumed[2] {true, true};
    std::mutex mutex[2]; // FIXME C++17 perhaps use shared_mutex (check if this causes starvation)
    std::condition_variable img_ready_cv[2];
    std::condition_variable img_consumed_cv[2];
    double buff_ts[2];
    mutable std::mutex rpc_methods_mutex;

    yarp::sig::ImageOf<yarp::sig::PixelRgb> background;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> rgb_image;
    yarp::os::Stamp stamp;

    void createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                         double& timestamp);

    bool makeSimpleBayer(yarp::sig::ImageOf<yarp::sig::PixelRgb>& src,
                         yarp::sig::ImageOf<yarp::sig::PixelMono>& bayer);

    void printTime(unsigned char* pixbuf, size_t pixbuf_w, size_t pixbuf_h, size_t x, size_t y, char* s, size_t size);
};

#endif // YARP_FAKEFRAMEGRABBER_FAKEFRAMEGRABBER_H
