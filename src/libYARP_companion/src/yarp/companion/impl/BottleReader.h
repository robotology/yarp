/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_COMPANION_IMPL_BOTTLEREADER_H
#define YARP_COMPANION_IMPL_BOTTLEREADER_H

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReader.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/BottleImpl.h>

using yarp::os::Bottle;
using yarp::os::Contact;
using yarp::os::ConnectionReader;
using yarp::os::Port;
using yarp::os::PortReader;
using yarp::os::Semaphore;
using yarp::os::impl::BottleImpl;

namespace yarp {
namespace companion {
namespace impl {

// used by Companion::cmdRead and Companion::cmdReadWrite

// just a temporary implementation until real ports are available
class BottleReader :
        public PortReader
{
private:
    yarp::os::Semaphore done;
    bool raw;
    bool env;
    std::string::size_type trim;
    Contact address;
public:
    Port core;

    BottleReader() : done(0) {
        raw = false;
        env = false;
        trim = std::string::npos;
        core.setReader(*this);
        core.setReadOnly();
    }

    void open(const char *name, bool showEnvelope, int trim_at = -1) {
        env = showEnvelope;
        trim = (trim_at > 0 ? static_cast<std::string::size_type>(trim_at) : std::string::npos);
        if (core.open(name)) {
            Companion::setActivePort(&core);
            address = core.where();
        } else {
            //yCError(COMPANION, "Could not create port");
            done.post();
        }
    }

    void wait() {
        done.wait();
        Companion::setActivePort(nullptr);
    }

    void showEnvelope() {
        if (env) {
            Bottle envelope;
            core.getEnvelope(envelope);
            if (envelope.size()>0) {
                yCInfo(COMPANION, "%s ", envelope.toString().c_str());
            }
        }
    }

    bool read(ConnectionReader& reader) override {
        BottleImpl bot;
        if (!reader.isValid()) {
            done.post();
            return false;
        }
        if (bot.read(reader)) {
            if (bot.size()==2 && bot.isInt32(0) && bot.isString(1) && !raw) {
                int code = bot.get(0).asInt32();
                if (code!=1) {
                    showEnvelope();
                    yCInfo(COMPANION, "%s", bot.get(1).asString().substr(0, trim).c_str());
                    fflush(stdout);
                }
                if (code==1) {
                    done.post();
                }
            } else {
                // raw = true; // don't make raw mode "sticky"
                showEnvelope();
                yCInfo(COMPANION, "%s", bot.toString().substr(0, trim).c_str());
                fflush(stdout);
            }
            return true;
        }
        return false;
    }

    void close() {
        core.close();
    }

    std::string getName() {
        return address.getRegName();
    }
};

} // namespace impl
} // namespace companion
} // namespace yarp

#endif // YARP_COMPANION_IMPL_BOTTLEREADER_H
