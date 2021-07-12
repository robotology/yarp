/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCorePackets.h>

#include <yarp/os/impl/LogComponent.h>

using yarp::os::impl::PortCorePacket;
using yarp::os::impl::PortCorePackets;

namespace {
YARP_OS_LOG_COMPONENT(PORTCOREPACKETS, "yarp.os.impl.PortCorePackets")
} // namespace

PortCorePackets::~PortCorePackets()
{
    while (!inactive.empty()) {
        delete inactive.back();
        inactive.pop_back();
    }
    while (!active.empty()) {
        delete active.back();
        active.pop_back();
    }
}

size_t PortCorePackets::getCount()
{
    return active.size();
}

PortCorePacket* PortCorePackets::getFreePacket()
{
    if (inactive.empty()) {
        PortCorePacket* obj = nullptr;
        obj = new PortCorePacket();
        yCAssert(PORTCOREPACKETS, obj != nullptr);
        inactive.push_back(obj);
    }
    PortCorePacket* next = inactive.front();
    if (next == nullptr) {
        yCError(PORTCOREPACKETS, "*** YARP consistency check failed.\n");
        yCError(PORTCOREPACKETS, "*** There has been a low-level failure in \"PortCorePackets\".\n");
        yCError(PORTCOREPACKETS, "*** This typically occurs when ports are accessed in a non-threadsafe way.\n");
        yCError(PORTCOREPACKETS, "*** For help: https://github.com/robotology/yarp/issues/new\n");
    }
    yCAssert(PORTCOREPACKETS, next != nullptr);
    inactive.remove(next);
    active.push_back(next);
    return next;
}

void PortCorePackets::freePacket(PortCorePacket* packet, bool clear)
{
    if (packet != nullptr) {
        if (clear) {
            packet->reset();
        }
        packet->completed = true;
        active.remove(packet);
        inactive.push_back(packet);
    }
}

bool PortCorePackets::completePacket(PortCorePacket* packet)
{
    if (packet != nullptr) {
        if (packet->getCount() <= 0) {
            packet->complete();
            return true;
        }
    }
    return false;
}

bool PortCorePackets::checkPacket(PortCorePacket* packet)
{
    if (packet != nullptr) {
        if (packet->getCount() <= 0) {
            packet->complete();
            freePacket(packet);
            return true;
        }
    }
    return false;
}
