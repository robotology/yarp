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

#include <sstream>
#include <iomanip>

using yarp::os::Bottle;
using yarp::os::Contact;
using yarp::os::ConnectionReader;
using yarp::os::Port;
using yarp::os::PortReader;
using yarp::os::Semaphore;
using yarp::os::impl::BottleImpl;

namespace yarp::companion::impl {

// used by Companion::cmdRead and Companion::cmdReadWrite

// just a temporary implementation until real ports are available
class BottleReader :
        public PortReader
{
private:
    yarp::os::Semaphore done;
    bool raw;
    Companion::showEnvelopeEnum eShowEnvelope;
    std::string::size_type trim;
    bool justOnce;
    Contact address;
public:
    Port core;

    BottleReader() : done(0) {
        raw = false;
        eShowEnvelope = Companion::showEnvelopeEnum::do_not_show;
        trim = std::string::npos;
        justOnce = false;
        core.setReader(*this);
        core.setReadOnly();
    }

    void open(const char *name, Companion::showEnvelopeEnum _showEnvelope, bool _justonce, int trim_at = -1) {
        eShowEnvelope = _showEnvelope;
        justOnce = _justonce;
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

    std::string showEnvelope() {
        if (eShowEnvelope != Companion::showEnvelopeEnum::do_not_show) {
            Bottle envelope;
            core.getEnvelope(envelope);
            if (envelope.size()>0)
            {
                //this check is bad way to test if the envelope is a actually a timestamp (yarp::os::Stamp)
                if (envelope.size() == 2 && envelope.get(0).isInt32() && envelope.get(1).isFloat64())
                {
                    std::ostringstream stream;
                    stream << std::fixed << std::setprecision(4) << envelope.get(1).asFloat64();
                    std::string formattedValue = stream.str();
                    return envelope.get(0).toString() + " " + formattedValue;
                }
                //or just a generic bottle
                else
                {
                    return envelope.toString();
                }
            }
        }
        return "";
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
                    if (eShowEnvelope == Companion::showEnvelopeEnum::do_not_show)
                    {
                        yCInfo(COMPANION, "%s", bot.get(1).asString().substr(0, trim).c_str());
                    }
                    else if (eShowEnvelope == Companion::showEnvelopeEnum::show_inline)
                    {
                        std::string envstring = showEnvelope();
                        yCInfo(COMPANION, "%s     %s", envstring.c_str(), bot.get(1).asString().substr(0, trim).c_str());
                    }
                    else if (eShowEnvelope == Companion::showEnvelopeEnum::show_two_lines)
                    {
                        std::string envstring = showEnvelope();
                        yCInfo(COMPANION, "%s", envstring.c_str());
                        yCInfo(COMPANION, "%s", bot.get(1).asString().substr(0, trim).c_str());
                    }
                    fflush(stdout);
                }
                if (code==1) {
                    done.post();
                }
            } else {
                // raw = true; // don't make raw mode "sticky"
                if (eShowEnvelope == Companion::showEnvelopeEnum::do_not_show)
                {
                    yCInfo(COMPANION, "%s", bot.toString().substr(0, trim).c_str());
                }
                if (eShowEnvelope == Companion::showEnvelopeEnum::show_inline)
                {
                    std::string envstring = showEnvelope();
                    yCInfo(COMPANION, "%s     %s", envstring.c_str(), bot.toString().substr(0, trim).c_str());
                }
                else if (eShowEnvelope == Companion::showEnvelopeEnum::show_two_lines)
                {
                    std::string envstring = showEnvelope();
                    yCInfo(COMPANION, "%s", envstring.c_str());
                    yCInfo(COMPANION, "%s", bot.toString().substr(0, trim).c_str());
                }
                fflush(stdout);
            }
            if (justOnce) {
                //this will close everything and terminate the execution after the first received data
                done.post();
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

} // namespace yarp::companion::impl

#endif // YARP_COMPANION_IMPL_BOTTLEREADER_H
