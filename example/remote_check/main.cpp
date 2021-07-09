/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Port.h>
#include <yarp/os/Property.h>
#include <yarp/os/Vocab.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/ControlBoardInterfaces.h>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

// a test remote frame grabber - provides blank images.
class FakeFrameGrabber : public Thread, PortReader {
private:
    Port p;
    Property prop;
public:

    virtual bool read(ConnectionReader& connection) {
        Bottle cmd, response;
        cmd.read(connection);
        printf("command received: %s\n", cmd.toString().c_str());
        int code = cmd.get(0).asVocab32();
        switch (code) {
        case yarp::os::createVocab32('s','e','t'):
            printf("set command received\n");
            prop.put(cmd.get(1).asString().c_str(),cmd.get(2));
            break;
        case yarp::os::createVocab32('g','e','t'):
            printf("get command received\n");
            response.addVocab32('i','s');
            response.add(cmd.get(1));
            response.add(prop.find(cmd.get(1).asString().c_str()));
            break;
        }
        if (response.size()>=1) {
            ConnectionWriter *writer = connection.getWriter();
            if (writer!=NULL) {
                response.write(*writer);
                printf("response sent: %s\n", response.toString().c_str());
            }
        }
    }

    virtual void beforeStart() {
        p.setReader(*this);
        p.open("/server");
    }

    virtual void run() {
        printf("Fake framegrabber starting\n");
        while (!isStopping()) {
            ImageOf<PixelRgb> img;
            img.resize(40,20);
            img.zero();
            printf("Fake framegrabber wrote an image...\n");
            p.write(img);
            Time::delay(1);
        }
        printf("Fake framegrabber stopping\n");
    }
};


int main() {
    Network::setLocalMode(true);

    FakeFrameGrabber fake;
    fake.start();

    PolyDriver dd("(device fakeFrameGrabber) (local /client) (remote /server)");

    if (!dd.isValid()) {
        printf("Device not available\n");
        exit(1);
    }

    printf("*** Device created\n");

    IFrameGrabberImage *grabber;
    dd.view(grabber);

    if (grabber!=NULL) {
        printf("*** It can supply images\n");
        ImageOf<PixelRgb> img;
        if (grabber->getImage(img)) {
            printf("*** Got a %dx%d image\n", img.width(), img.height());
        } else {
            printf("*** Failed to actually read an image\n");
        }

        IFrameGrabberControls *ctrl;
        dd.view(ctrl);
        if (ctrl!=NULL) {
            printf("*** It can be controlled as a framegrabber\n");
            double x = ctrl->getBrightness();
            printf("*** brightness before setting is reported as %g\n", x);
            ctrl->setBrightness(100);
            printf("*** brightness set\n");
            x = ctrl->getBrightness();
            printf("*** brightness after setting reported as %g\n", x);
        } else {
            printf("*** It can <<<<<NOT>>>>> be controlled as a framegrabber\n");
        }

    } else {
        printf("*** It can <<<<<NOT>>>>> supply images\n");
    }

    IPidControl *pid;
    dd.view(pid);

    if (pid!=NULL) {
        printf("*** It can do PID control\n");
    } else {
        printf("*** It can <<<<<NOT>>>>> do PID control\n");
    }

    return 0;
}
