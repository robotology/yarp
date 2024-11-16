/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "GstreamerStream.h"

#include <yarp/conf/system.h>
#include <yarp/conf/environment.h>

#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>

#include <yarp/os/LogComponent.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <iostream>

#include <cerrno>
#include <cstring>

using namespace yarp::os::impl;
using namespace yarp::os;


YARP_LOG_COMPONENT(GSTREAMER_STREAM,
                   "yarp.carrier.gstreamer.gstreamerStream",
                   yarp::os::Log::minimumPrintLevel(),
                   yarp::os::Log::LogTypeReserved,
                   yarp::os::Log::printCallback(),
                   nullptr)

bool GstreamerStream::open(const std::string& pipeline_string, const Contact& remote)
{
    Contact local = Contact("127.0.0.1", -1);
    return open(pipeline_string, local, remote);
}

bool GstreamerStream::open(const std::string& pipeline_string, const Contact& local, const Contact& remote)
{
    m_localAddress = local;
    m_remoteAddress = remote;

    //yCDebug(GSTREAMER_STREAM, "Update: GstreamerStream::open() from %s to %s", m_localAddress.toURI().c_str(), m_remoteAddress.toURI().c_str());

    GstYarpDecoder_cfgParamters params;
    m_decoder = new GstYarpDecoder(&m_mutex, &m_sema, params);

    bool ret = false;
    ret = m_decoder->init(pipeline_string);
    if (!ret)
    {
        return false;
    }
    ret = m_decoder->start();
    if (!ret)
    {
        return false;
    }

    return true;
}

GstreamerStream::~GstreamerStream()
{
    closeMain();
}

void GstreamerStream::interrupt()
{
    bool act = false;
    m_mutex.lock();
    if ((!m_closed) && (!m_interrupting))
    {
        act = true;
        m_interrupting = true;
        m_closed = true;
    }
    m_mutex.unlock();

    // wait for interruption to be done
    if (m_interrupting)
    {
        while (m_interrupting)
        {
            yCDebug(GSTREAMER_STREAM, "waiting for interrupt to be finished...");
            yarp::os::SystemClock::delaySystem(0.1);
        }
    }
}

void GstreamerStream::closeMain()
{
    if (m_decoder)
    {
        m_decoder->stop();

        delete m_decoder;
        m_decoder = nullptr;
    }
}

bool GstreamerStream::isOk() const
{
    return true;
}

//once that you enter here, it is guaranteed that frame will be not modified until prepareNextFrame() is called.
yarp::sig::ImageOf<yarp::sig::PixelRgb>* GstreamerStream::getFrame()
{
    //create a fake frame, just for test
    if (m_debug_test_image_generation)
    {
        static yarp::sig::ImageOf<yarp::sig::PixelRgb> frame;
        frame.resize(640, 480);
        static int val = 0;
        if (val) {
            memset(frame.getRawImage(), 90, frame.getRawImageSize());
            val = 0;
        } else {
            memset(frame.getRawImage(), 200, frame.getRawImageSize());
            val = 1;
        }
        return &frame;
    }

    //get the frame from the decoder.
    //this call is blocking until the frame is ready.
    return m_decoder->getLastFrame();
}

void GstreamerStream::prepareNextFrame()
{
     if (m_debug_test_image_generation)
     {
        return;
     }
     return m_decoder->prepareNextFrame();
}

yarp::conf::ssize_t GstreamerStream::read(Bytes& b)
{
    size_t bl = b.length();

    if (m_enum_phases == enum_phases::PHASE_0_GET_IMG)
    {
        m_pointer_last_frame = getFrame();
        m_enum_phases = enum_phases::PHASE_1_PREPARE_HEADER;
    }
    if (m_enum_phases == enum_phases::PHASE_1_PREPARE_HEADER)
    {
        m_imgHeader.setFromImage(*m_pointer_last_frame);
        m_enum_phases = enum_phases::PHASE_2_SEND_HEADER;
    }
    if (m_enum_phases == enum_phases::PHASE_2_SEND_HEADER)
    {
        sendHeaderLabel:
        // starting to send the header
        if (m_remaining == 0)
        {
            // cursor is set to the header, which has lenght = remaining
            m_cursor = (char*)(&m_imgHeader);
            m_remaining = sizeof(m_imgHeader);
        }

        size_t bytestobecopied = m_remaining;
        if (bytestobecopied > bl)
        {
            bytestobecopied = bl;
            memcpy(b.get(), m_cursor, bytestobecopied);
            m_remaining -= bytestobecopied;
            m_cursor += bytestobecopied;
            return bytestobecopied;
        }
        else
        {
            memcpy(b.get(), m_cursor, bytestobecopied);
            m_remaining = 0;
            m_enum_phases = enum_phases::PHASE_3_SEND_IMAGE;
            return bytestobecopied;
        }
    }
    if (m_enum_phases == enum_phases::PHASE_3_SEND_IMAGE)
    {
        // starting to send the image
        if (m_remaining == 0) {
            // cursor is set to the image, which has lenght = remaining
            m_cursor = (char*)(m_pointer_last_frame->getRawImage());
            m_remaining = m_pointer_last_frame->getRawImageSize();
        }

        size_t bytestobecopied = m_remaining;
        if (bytestobecopied > bl)
        {
            bytestobecopied = bl;
            memcpy(b.get(), m_cursor, bytestobecopied);
            m_remaining -= bytestobecopied;
            m_cursor += bytestobecopied;
            return bytestobecopied;
        }
        else
        {
            memcpy(b.get(), m_cursor, bytestobecopied);
            m_remaining = 0;
            m_enum_phases = enum_phases::PHASE_0_GET_IMG;
            this->prepareNextFrame();
            return bytestobecopied;
        }
    }

    // unreachable code
    yError("Logic bug 2");
    yAssert(false);
    return -1;
}

void GstreamerStream::write(const Bytes& b)
{
}

void GstreamerStream::reset()
{
}

void GstreamerStream::beginPacket()
{
}

void GstreamerStream::endPacket()
{
}
