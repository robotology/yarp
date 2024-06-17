/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_GSTREAMERTREAM_H
#define YARP_GSTREAMERTREAM_H

#include <yarp/os/Contact.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/TwoWayStream.h>
#include "GstreamerDecoder.h"

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <cstdlib>
#include <mutex>

class GstreamerStream :
        public yarp::os::TwoWayStream,
        public yarp::os::InputStream,
        public yarp::os::OutputStream
{
private:
    GstYarpDecoder* m_decoder = nullptr;

public:
    GstreamerStream() :
            m_closed(false),
            m_interrupting(false),
            m_mutex()
    {
    }

    virtual bool open(const std::string& pipeline_string, const yarp::os::Contact& remote);

    virtual bool open(const std::string& pipeline_string, const yarp::os::Contact& local, const yarp::os::Contact& remote);

    virtual ~GstreamerStream();

    InputStream& getInputStream() override
    {
        return *this;
    }

    OutputStream& getOutputStream() override
    {
        return *this;
    }

    const yarp::os::Contact& getLocalAddress() const override
    {
        return m_localAddress;
    }

    const yarp::os::Contact& getRemoteAddress() const override
    {
        return m_remoteAddress;
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb>* getFrame();
    void prepareNextFrame();

    void interrupt() override;

    void close() override
    {
        closeMain();
    }

    virtual void closeMain();

    bool isOk() const override;

    using yarp::os::InputStream::read;
    yarp::conf::ssize_t read(yarp::os::Bytes& b) override;

    using yarp::os::OutputStream::write;
    void write(const yarp::os::Bytes& b) override;

    void reset() override;

    void beginPacket() override;

    void endPacket() override;

private:
    bool m_closed, m_interrupting;

    yarp::sig::ImageOf<yarp::sig::PixelRgb>* m_pointer_last_frame=nullptr;
    yarp::sig::ImageNetworkHeader m_imgHeader;
    enum class enum_phases
    {
        PHASE_0_GET_IMG = 0,
        PHASE_1_PREPARE_HEADER = 1,
        PHASE_2_SEND_HEADER = 2,
        PHASE_3_SEND_IMAGE = 3
    } m_enum_phases = enum_phases::PHASE_0_GET_IMG;

    bool m_debug_test_image_generation = false;

    yarp::os::Contact m_localAddress, m_remoteAddress;
    std::mutex m_mutex;
    yarp::os::Semaphore m_sema;

    char* m_cursor = nullptr;
    size_t m_remaining = 0;

};

#endif
