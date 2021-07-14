/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP2_WIREBOTTLE
#define YARP2_WIREBOTTLE

#include <yarp/os/SizedWriter.h>

#include <yarp/wire_rep_utils/api.h>

namespace yarp {
namespace wire_rep_utils {

class YARP_wire_rep_utils_API SizedWriterTail :
        public yarp::os::SizedWriter
{
private:
    yarp::os::SizedWriter *delegate;
    size_t payload_index, payload_offset;
public:
    void setDelegate(yarp::os::SizedWriter *delegate, int index,
                     int offset) {
        this->delegate = delegate;
        payload_index = index;
        payload_offset = offset;
    }

    size_t length() const override {
        return delegate->length()-payload_index;
    }

    size_t headerLength() const override {
        return 0; // not supported
    }

    size_t length(size_t index) const override {
        index += payload_index;
        if (index == payload_index) {
            return delegate->length(index) - payload_offset;
        }
        return delegate->length(index);
    }

    const char *data(size_t index) const override {
        index += payload_index;
        if (index == payload_index) {
            return delegate->data(index) + payload_offset;
        }
        return delegate->data(index);
    }

    yarp::os::PortReader *getReplyHandler() override {
        return delegate->getReplyHandler();
    }

    yarp::os::Portable *getReference() override {
        return delegate->getReference();
    }

    bool dropRequested() override { return false; }


    void startWrite() const override {
    }

    void stopWrite() const override {
    }
};

class YARP_wire_rep_utils_API WireBottle
{
public:
    static bool checkBottle(void *cursor, int len);
    static bool extractBlobFromBottle(yarp::os::SizedWriter& src,
                                      SizedWriterTail& dest);
};

} // namespace wire_rep_utils
} // namespace yarp

#endif
