// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_SERVERFRAMEGRABBER_
#define _YARP2_SERVERFRAMEGRABBER_

#include <stdio.h>

#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>


namespace yarp {
    namespace dev {
        class ServerFrameGrabber;
    }
}

#define VOCAB_BRIGHTNESS VOCAB3('b','r','i')
#define VOCAB_SHUTTER VOCAB4('s','h','u','t')
#define VOCAB_GAIN VOCAB4('g','a','i','n')
#define VOCAB_SET VOCAB3('s','e','t')
#define VOCAB_GET VOCAB3('g','e','t')
#define VOCAB_IS VOCAB2('i','s')
#define VOCAB_WIDTH VOCAB1('w')
#define VOCAB_HEIGHT VOCAB1('h')



/**
 * Export a frame grabber to the network, providing the 
 * IFrameGrabberImage, IFrameGrabberControls, and IAudioGrabberSound
 * interfaces.  The corresponding client is a RemoteFrameGrabber.
 *
 * The network interface is a single Port.
 * Images are streamed out from that Port -- RemoteFrameGrabber
 * uses this stream to provide the IFrameGrabberImage interface.
 * The IFrameGrabberControls functionality is provided via RPC.
 *
 * Here's a command-line example:
 * \verbatim
   [terminal A] yarpdev --device test_grabber --width 8 --height 8 --name /grabber --period 2
   [terminal B] yarp read /read
   [terminal C] yarp connect /grabber /read
   [terminal C] echo "[get] [gain]" | yarp rpc /grabber
   \endverbatim
 * The yarpdev line starts a TestFrameGrabber wrapped in a ServerFrameGrabber.
 * After the "yarp connect" line, image descriptions will show up in 
 * terminal B (you could view them with the yarpview application).
 * The "yarp rpc" command should query the gain (0.0 for the test grabber).
 *
 * <TABLE>
 * <TR><TD> Command (text form) </TD><TD> Response </TD><TD> Code equivalent </TD></TR>
 * <TR><TD> [set] [bri] 1.0 </TD><TD> none </TD><TD> setBrightness() </TD></TR>
 * <TR><TD> [set] [gain] 1.0 </TD><TD> none </TD><TD> setGain() </TD></TR>
 * <TR><TD> [set] [shut] 1.0 </TD><TD> none </TD><TD> setShutter() </TD></TR>
 * <TR><TD> [get] [bri] </TD><TD> [is] [bri] 1.0 </TD><TD> getBrightness() </TD></TR>
 * <TR><TD> [get] [gain] </TD><TD> [is] [gain] 1.0 </TD><TD> getGain() </TD></TR>
 * <TR><TD> [get] [shut] </TD><TD> [is] [shut] 1.0 </TD><TD> getShutter() </TD></TR>
 * </TABLE>
 *
 */
class yarp::dev::ServerFrameGrabber : public DeviceDriver, 
            private yarp::os::Thread,
            public yarp::os::PortReader,
            public IFrameGrabberImage, public IFrameGrabberControls,
            public IAudioGrabberSound
            // convenient to put these here just to make sure all
            // methods get implemented
{
private:
    bool spoke;
    yarp::os::Port p;
    yarp::os::PortWriterBuffer<yarp::sig::ImageOf<yarp::sig::PixelRgb> > writer;
    yarp::os::PortWriterBuffer<yarp::sig::Sound> writerSound;
    PolyDriver poly;
    IFrameGrabberImage *fgImage;
    IAudioGrabberSound *fgSound;
    IFrameGrabberControls *fgCtrl;
    yarp::os::Property settings;
public:
    /**
     * Constructor.
     */
    ServerFrameGrabber() {
        fgImage = NULL;
        fgSound = NULL;
        fgCtrl = NULL;
		spoke = false;
    }
    
    virtual bool close() {
        return true;
    }
    
   /**
     * Configure with a set of options. These are:
     * <TABLE>
     * <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
     * <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
     * </TABLE>
     *
     * @param config The options to use
     * @return true iff the object could be configured.
     */
    virtual bool open(yarp::os::Searchable& config) {
        p.setReader(*this);
        
        yarp::os::Value *name;
        if (config.check("subdevice",name)) {
            printf("Subdevice %s\n", name->toString().c_str());
            if (name->isString()) {
                // maybe user isn't doing nested configuration
                yarp::os::Property p;
                p.fromString(config.toString());
                p.put("device",name->toString());
                poly.open(p);
            } else {
                poly.open(*name);
            }
            if (!poly.isValid()) {
                printf("cannot make <%s>\n", name->toString().c_str());
            }
        } else {
            printf("\"--subdevice <name>\" not set for server_framegrabber\n");
            return false;
        }
        if (poly.isValid()) {
            poly.view(fgImage);
            poly.view(fgSound);
            poly.view(fgCtrl);
        }

        if (fgImage!=NULL) {
            writer.attach(p);
        } else {
            writerSound.attach(p);
        }

        if (config.check("name",name)) {
            p.open(name->asString());
        } else {
            p.open("/grabber");
        }

        if (fgImage!=NULL||fgSound!=NULL) {
            start();
            return true;
        }
        printf("subdevice <%s> doesn't look like a framegrabber\n",
               name->toString().c_str());
        return false;
    }

    virtual void run() {
        printf("Server grabber starting\n");
        while (!isStopping()) {
            if (fgImage!=NULL) {
                // for now, sound and image are mutually exclusive
                yarp::sig::ImageOf<yarp::sig::PixelRgb>& img = writer.get();
                getImage(img);
                if (!spoke) {
                    printf("Network framegrabber writing a %dx%d image...\n",
                           img.width(),img.height());
                    spoke = true;
                }
                writer.write();
            } else {
                // for now, sound and image are mutually exclusive
                yarp::sig::Sound& snd = writerSound.get();
                getSound(snd);
                if (!spoke) {
                    printf("Network framegrabber writing a %dx%d sound...\n",
                           snd.getSamples(),snd.getChannels());
                    spoke = true;
                }
                writerSound.write();
            }
        }
        printf("Server grabber stopping\n");
    }

    virtual bool read(ConnectionReader& connection) {
        yarp::os::Bottle cmd, response;
        cmd.read(connection);
        printf("command received: %s\n", cmd.toString().c_str());
        int code = cmd.get(0).asVocab();
        switch (code) {
        case VOCAB_SET:
            printf("set command received\n");
            {
                bool ok = false;
                switch(cmd.get(1).asVocab()) {
                case VOCAB_BRIGHTNESS:
                    ok = setBrightness(cmd.get(2).asDouble());
                    break;
                case VOCAB_SHUTTER:
                    ok = setShutter(cmd.get(2).asDouble());
                    break;
                case VOCAB_GAIN:
                    ok = setGain(cmd.get(2).asDouble());
                    break;
                }
            }
            break;
        case VOCAB_GET:
            printf("get command received\n");
            {
                bool ok = false;
                response.addVocab(VOCAB_IS);
                response.add(cmd.get(1));
                switch(cmd.get(1).asVocab()) {
                case VOCAB_BRIGHTNESS:
                    ok = true;
                    response.addDouble(getBrightness());
                    break;
                case VOCAB_SHUTTER:
                    ok = true;
                    response.addDouble(getShutter());
                    break;
                case VOCAB_GAIN:
                    ok = true;
                    response.addDouble(getGain());
                    break;
                case VOCAB_WIDTH:
                    // normally, this would come from stream information
                    ok = true;
                    response.addInt(width());
                    break;
                case VOCAB_HEIGHT:
                    // normally, this would come from stream information
                    ok = true;
                    response.addInt(height());
                    break;
                }
                if (!ok) {
                    // leave answer blank
                }
            }
            break;
        }
        if (response.size()>=1) {
            ConnectionWriter *writer = connection.getWriter();
            if (writer!=NULL) {
                response.write(*writer);
                printf("response sent: %s\n", response.toString().c_str());
            }
        }
        return true;
    }

    virtual bool getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
        if (fgImage==NULL) { return false; }
        return fgImage->getImage(image);
    }
    
    virtual bool getSound(yarp::sig::Sound& sound) {
        if (fgSound==NULL) { return false; }
        return fgSound->getSound(sound);
    }
    
    virtual int height() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->height();
    }

    virtual int width() const {
        if (fgImage==NULL) { return 0; }
        return fgImage->width();
    }

    virtual bool setBrightness(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setBrightness(v);
    }

    virtual bool setShutter(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setShutter(v);
    }

    virtual bool setGain(double v) {
        if (fgCtrl==NULL) { return false; }
        return fgCtrl->setGain(v);
    }

    virtual double getBrightness() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getBrightness();
    }

    virtual double getShutter() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getShutter();
    }

    virtual double getGain() const {
        if (fgCtrl==NULL) { return 0; }
        return fgCtrl->getGain();
    }
};

#endif

