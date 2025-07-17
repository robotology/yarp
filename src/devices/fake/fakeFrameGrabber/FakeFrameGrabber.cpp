/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/ImageDraw.h>

#include <cstdio>
#include <thread>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace {
YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")
} // namespace

bool FakeFrameGrabber::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle command;
    yarp::os::Bottle reply;
    bool ok = command.read(connection);
    if (!ok) {
        return false;
    }
    reply.clear();

    if (command.get(0).asString()=="help")
    {
        reply.addVocab32("many");
        reply.addString("set_mode <mode>");
        reply.addString("set_image <file_name>/off");
        reply.addString("available modes: ball, line, grid, size, rand, none, time");
        reply.addString("set_noise on/off");
        reply.addString("set_snr <snr>");
        reply.addString("");
    }
    else if (command.get(0).asString() == "set_mode")
    {
        m_mode= command.get(1).asString();
        reply.addString("ack");
    }
    else if (command.get(0).asString() == "set_image")
    {
        if (command.get(1).asString() == "off")
        {
            m_have_bg=false;
            reply.addString("ack");
        }
        else
        {
            if (yarp::sig::file::read(background, command.get(1).asString()))
            {
                m_width = background.width();
                m_height = background.height();
                m_have_bg = true;
                reply.addString("ack");
            }
            else
            {
                m_have_bg = false;
                reply.addString("err");
            }
        }
    }
    else if (command.get(0).asString() == "set_noise")
    {
        if (command.get(1).asString() == "off") {
            m_add_noise = false;
            reply.addString("ack");
        } else if (command.get(1).asString() == "on") {
            m_add_noise = true;
            reply.addString("ack");
        } else {
            reply.addString("err");
        }
    }
    else if (command.get(0).asString() == "set_snr")
    {
        m_snr = yarp::conf::clamp(command.get(1).asFloat64(), 0.0, 1.0);
        reply.addString("ack");
    }
    else
    {
        reply.addString("Unknown command. Type 'help'.");
    }

    yarp::os::ConnectionWriter* returnToSender = connection.getWriter();
    if (returnToSender != nullptr)
    {
        reply.write(*returnToSender);
    }

    return true;
}

bool FakeFrameGrabber::close() {
    stop();
    m_rpcPort.close();
    return true;
}

bool FakeFrameGrabber::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    m_intrinsic.put("physFocalLength",m_physFocalLength);
    m_intrinsic.put("focalLengthX",m_focalLengthX);
    m_intrinsic.put("focalLengthY",m_focalLengthY);
    m_intrinsic.put("principalPointX",m_principalPointX);
    m_intrinsic.put("principalPointY",m_principalPointY);

    Value* val = Value::makeList();
    auto* bb = val->asList();
    for (double num : m_rectificationMatrix) { bb->addFloat64(num); }
    m_intrinsic.put("rectificationMatrix",val);

    m_intrinsic.put("distortionModel", m_distortionModel);
    m_intrinsic.put("k1",m_k1);
    m_intrinsic.put("k2",m_k2);
    m_intrinsic.put("k3",m_k3);
    m_intrinsic.put("t1",m_t1);
    m_intrinsic.put("t2",m_t2);
    //Only for debug
    CameraConfig conf1;
    conf1.height=128;
    conf1.width=128;
    conf1.framerate=60.0;
    conf1.pixelCoding=VOCAB_PIXEL_RGB;
    configurations.push_back(conf1);

    CameraConfig conf2;
    conf2.height=256;
    conf2.width=256;
    conf2.framerate=30.0;
    conf2.pixelCoding=VOCAB_PIXEL_BGR;
    configurations.push_back(conf2);

    CameraConfig conf3;
    conf3.height=512;
    conf3.width=512;
    conf3.framerate=15.0;
    conf3.pixelCoding=VOCAB_PIXEL_MONO;
    configurations.push_back(conf3);

    if (m_freq>0)
    {
        m_period = 1/ m_freq;
    }
    else if (m_period>0)
    {
        //ok
    }
    else
    {
        yCWarning(FAKEFRAMEGRABBER, "Either `period` or `freq` parameters must be a valid >0 value");
    }

    if (!m_src.empty())
    {
        if (!yarp::sig::file::read(background, m_src))
        {
            yCWarning(FAKEFRAMEGRABBER, "Unable to open file");
            return false;
        }
    }
    if (background.width()>0)
    {
        if (m_width>0 || m_height>0)
        {
            yCWarning(FAKEFRAMEGRABBER, "width and height options are ignored when passing a background image");
        }
        m_width = background.width();
        m_height = background.height();
        m_have_bg = true;
    }


    m_snr = yarp::conf::clamp(m_snr, 0.0, 1.0);
    m_mono = m_mono || m_bayer;

    yCInfo(FAKEFRAMEGRABBER,
               "Test grabber period %g / freq %g , mode %s",
               m_period,
               (1.0/m_period),
               m_mode.c_str());

    m_bx = m_width/2;
    m_by = m_height/2;

    for (auto& buff : buffs) {
        buff.resize(m_width, m_height);
        buff.zero();
    }

    if (!m_rpcPort.open(m_fakeFrameGrabber_rpc_port.c_str()))
    {
        yCError(FAKEFRAMEGRABBER, "Failed to open port %s", m_fakeFrameGrabber_rpc_port.c_str());
        yCError(FAKEFRAMEGRABBER, "Do you have multiple FakeFrameGrabber devices running?");
        yCError(FAKEFRAMEGRABBER, "If yes, use the `fakeFrameGrabber_rpc_port` parameter to set a different name for each of them");
        return false;
    }
    m_rpcPort.setReader(*this);

    start();

    return true;
}

void FakeFrameGrabber::timing() {
    double now = yarp::os::Time::now();

    if (now-prev>1000) {
        prev = now;
    }
    double dt = m_period-(now-prev);

    if (dt>0) {
        yarp::os::Time::delay(dt);
    }

    // this is the controlled instant when we consider the
    // image as going out
    prev += m_period;
}

int FakeFrameGrabber::height() const
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return m_height;
}

int FakeFrameGrabber::width() const
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return m_width;
}

void FakeFrameGrabber::run()
{
    while (!isStopping()) {
        for (size_t i = 0; i < 2 && !isStopping(); ++i) {
            if (!m_syncro) {
                std::unique_lock<std::mutex> lk(mutex[i]);
                createTestImage(buffs[i], buff_ts[i]);
                timing();
                lk.unlock();

                curr_buff_mutex.lock();
                curr_buff = i;
                curr_buff_mutex.unlock();
                std::this_thread::yield();
            } else {
                std::unique_lock<std::mutex> lk(mutex[i]);
                img_consumed_cv[i].wait(lk, [&]{ if (img_ready[i]) {img_ready_cv[i].notify_one();} return (isStopping() || img_consumed[i]);});
                if (isStopping()) {
                    break;
                }
                img_ready[i] = false;
                img_consumed[i] = false;
                createTestImage(buffs[i], buff_ts[i]);
                img_ready[i] = true;
                img_ready_cv[i].notify_all();
            }
        }
    }
}

void FakeFrameGrabber::onStop()
{
    // Unlock any blocked thread.
    if (m_syncro) {
        for (size_t i = 0; i < 2; ++i) {
            std::unique_lock<std::mutex> lk(mutex[i]);
            img_consumed[i] = true;
            img_consumed_cv[i].notify_all();
            img_ready[i] = true;
            img_ready_cv[i].notify_all();
        }
    }
}


ReturnValue FakeFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    if (!isRunning()) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    if (!m_syncro) {
        curr_buff_mutex.lock();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        // Release the mutex after we get the lock on current image to avoid
        // that the image is swapped while we are waiting to read it
        curr_buff_mutex.unlock();
        image.copy(buffs[cb]);
        stamp.update(buff_ts[cb]);
    } else {
        curr_buff_mutex.lock();
        timing();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        img_ready_cv[cb].wait(lk, [&]{return (!isRunning() || img_ready[cb]);});
        if (!isRunning()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }

        image.copy(buffs[cb]);
        stamp.update(buff_ts[cb]);
        img_consumed[cb] = true;
        img_consumed_cv[cb].notify_one();

        curr_buff = (cb + 1) % 2;
        curr_buff_mutex.unlock();
    }

    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getImage(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if (!isRunning()) {
        return ReturnValue::return_code::return_value_error_not_ready;
    }

    if (!m_syncro) {
        curr_buff_mutex.lock();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        curr_buff_mutex.unlock();
        if (m_bayer) {
            makeSimpleBayer(buffs[cb], image);
        } else {
            image.copy(buffs[cb]);
        }
        stamp.update(buff_ts[cb]);
    } else {
        curr_buff_mutex.lock();
        timing();
        size_t cb = curr_buff;
        std::unique_lock<std::mutex> lk(mutex[cb]);
        img_ready_cv[cb].wait(lk, [&]{return (!isRunning() || img_ready[cb]);});
        if (!isRunning()) {
            return ReturnValue::return_code::return_value_error_not_ready;
        }
        if (m_bayer) {
            makeSimpleBayer(buffs[cb], image);
        } else {
            image.copy(buffs[cb]);
        }
        stamp.update(buff_ts[cb]);
        img_consumed[cb] = true;
        img_consumed_cv[cb].notify_one();

        curr_buff = (cb + 1) % 2;
        curr_buff_mutex.unlock();
    }

    return ReturnValue_ok;
}

ReturnValue FakeFrameGrabber::getImageCrop(cropType_id_t cropType,
                                    std::vector<yarp::dev::vertex_t> vertices,
                                    yarp::sig::ImageOf<yarp::sig::PixelRgb>& image)
{
    yCDebugThrottle(FAKEFRAMEGRABBER, 5.0) << "Hardware crop requested!";
    return yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelRgb>>::getImageCrop(cropType, vertices, image);
}

ReturnValue FakeFrameGrabber::getImageCrop(cropType_id_t cropType,
                                    std::vector<yarp::dev::vertex_t> vertices,
                                    yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    yCDebugThrottle(FAKEFRAMEGRABBER, 5.0) << "Hardware crop requested!";
    return yarp::dev::IFrameGrabberOf<yarp::sig::ImageOf<yarp::sig::PixelMono>>::getImageCrop(cropType, vertices, image);
}

yarp::os::Stamp FakeFrameGrabber::getLastInputStamp()
{
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);
    return stamp;
}
