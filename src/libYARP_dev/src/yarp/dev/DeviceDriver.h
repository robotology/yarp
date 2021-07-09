/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_DEVICEDRIVER_H
#define YARP_DEV_DEVICEDRIVER_H

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/IConfig.h>

#include <yarp/dev/api.h>

namespace yarp {
    /**
     * An interface for the device drivers
     */
    namespace dev {
        class DeviceDriver;
        class DeprecatedDeviceDriver;
        class DeviceResponder;
    }
}

/**
 * \ingroup dev_class
 *
 * Interface implemented by all device drivers.
 */
class YARP_dev_API yarp::dev::DeviceDriver : public yarp::os::IConfig
{
public:
    /**
     * Destructor.
     */
    ~DeviceDriver() override = default;

    /**
     * Open the DeviceDriver.
     * @param config is a list of parameters for the device.
     * Which parameters are effective for your device can vary.
     * See \ref dev_examples "device invocation examples".
     * If there is no example for your device,
     * you can run the "yarpdev" program with the verbose flag
     * set to probe what parameters the device is checking.
     * If that fails too,
     * you'll need to read the source code (please nag one of the
     * yarp developers to add documentation for your device).
     * @return true/false upon success/failure
     */
    bool open(yarp::os::Searchable& config) override { YARP_UNUSED(config); return true; }

    /**
     * Close the DeviceDriver.
     * @return true/false on success/failure.
     */
    bool close() override { return true; }


    /**
     * Get an interface to the device driver.

     * @param x A pointer of type T which will be set to point to this
     * object if that is possible.

     * @return true iff the desired interface is implemented by
     * the device driver.
     */
    template <class T>
    bool view(T *&x) {
        x = nullptr;

        // This is not super-portable; and it requires RTTI compiled
        // in.  For systems on which this is a problem, suggest:
        // either replace it with a regular cast (and warn user) or
        // implement own method for checking interface support.
        T *v = dynamic_cast<T *>(getImplementation());

        if (v != nullptr) {
            x = v;
            return true;
        }

        return false;
    }

    /**
     * Some drivers are bureaucrats, pointing at others.  Such drivers override
     * this method.
     *
     * @return "real" device driver
     *
     */
    virtual DeviceDriver *getImplementation() {
        return this;
    }
};

/**
 * \ingroup dev_class
 *
 * Interface implemented by deprecated device drivers.
 *
 * When a device is deprecated, replace yarp::dev::DeviceDriver with
 * yarp::dev::DeprecatedDeviceDriver in the list of interfaces implemented by
 * the class, in order to let YARP know that the class is deprecated.
 *
 * Deprecated device drivers cannot be opened as PolyDriver unless the
 * "allow-deprecated-devices" option is passed in the configuration.
 */
class YARP_dev_API yarp::dev::DeprecatedDeviceDriver : virtual public yarp::dev::DeviceDriver
{
};


/**
 * \ingroup dev_class
 *
 * A cheap and cheerful framework for human readable/writable forms of
 * messages to devices.  The main concern is to makes it easier to
 * document these messages.  Override DeviceResponder::respond() to
 * respond to new messages.  You don't need to use this class --
 * the network format of messages is defined independently of it.
 */
class YARP_dev_API yarp::dev::DeviceResponder : public yarp::os::PortReader,
            public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{

private:
    yarp::os::Bottle examples;
    yarp::os::Bottle explains;
    yarp::os::Bottle details;

public:
    /**
     * Constructor
     */
    DeviceResponder();

    /**
     * Add information about a message that the respond() method
     * understands.
     * @param txt the message, in text form
     * @param explain an (optional) what the message means
     */
    void addUsage(const char *txt, const char *explain = nullptr);

    /**
     * Add information about a message that the respond() method
     * understands.
     * @param bot the message, in bottle form
     * @param explain an (optional) what the message means
     */
    void addUsage(const yarp::os::Bottle& bot, const char *explain = nullptr);

    /**
     * Respond to a message.
     * @param command the message
     * @param reply the response
     * @return true if there was no critical failure
     */
    virtual bool respond(const yarp::os::Bottle& command,
                         yarp::os::Bottle& reply);

    /**
     * Handler for reading messages from the network, and passing
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    bool read(yarp::os::ConnectionReader& connection) override;

    using yarp::os::TypedReaderCallback<yarp::os::Bottle>::onRead;
    /**
     * Alternative handler for reading messages from the network, and passing
     * them on to the respond() method.  There can be no replies made
     * if this handler is used.
     * @param v the message
     */
    void onRead(yarp::os::Bottle& v) override {
        yarp::os::Bottle reply;
        respond(v,reply);
    }

    /**
     * Regenerate usage information.
     */
    void makeUsage();

    /**
     * Attach this object to a source of messages.
     * @param source a BufferedPort or PortReaderBuffer that
     * receives data.
     */
    void attach(yarp::os::TypedReader<yarp::os::Bottle>& source) {
        source.useCallback(*this);
        source.setReplier(*this);
    }
};


#endif // YARP_DEV_DEVICEDRIVER_H
