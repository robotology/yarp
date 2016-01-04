// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <deque>

#include <stdio.h>
#include <math.h>

#include <yarp/os/all.h>
#include <yarp/os/Log.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

#include <yarp/sig/SoundFile.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::dev;

int padding = 0;

class Echo : public TypedReaderCallback<Sound> {
private:
    PolyDriver poly;
    IAudioRender *put;
    BufferedPort<Sound> port;
    Semaphore mutex;
    bool muted;
    bool saving;
    std::deque<Sound> sounds;
    int samples;
    int channels;

public:
    Echo() : mutex(1) {
        put = NULL;
        port.useCallback(*this);
        port.setStrict();
        muted = false;
        saving = false;
        samples = 0;
        channels = 0;
        put = NULL;
    }

    bool open(Searchable& p) {
        bool dev = true;
        if (p.check("nodevice")) {
            dev = false;
        }
        if (dev) {
            poly.open(p);
            if (!poly.isValid()) {
                yError("cannot open driver\n");
                return false;
            }

            if (!p.check("mute")) {
                // Make sure we can write sound
                poly.view(put);
                if (put==NULL) {
                    yError("cannot open interface\n");
                    return false;
                }
            }
        }

        port.setStrict(true);
        if (!port.open(p.check("name",Value("/yarphear")).asString())) {
            yError("Communication problem\n");
            return false;
        }

        if (p.check("remote")) {
            Network::connect(p.check("remote",Value("/remote")).asString(),
                             port.getName());
        }

        return true;
    }

    void onRead(Sound& sound)
     {
        #ifdef TEST
        //this block can be used to measure time elapsed between two sound packets
        static double t1= yarp::os::Time::now();
        static double t2= yarp::os::Time::now();
        t1= yarp::os::Time::now();
        yDebug("onread %f\n", t2-t1);
        t2 = yarp::os::Time::now();
        #endif

        int ct = port.getPendingReads();
        //yDebug("pending reads %d\n", ct);
        while (ct>padding) {
            ct = port.getPendingReads();
            yWarning("Dropping sound packet -- %d packet(s) behind\n", ct);
            port.read();
        }
        mutex.wait();
        /*
          if (muted) {
          for (int i=0; i<sound.getChannels(); i++) {
          for (int j=0; j<sound.getSamples(); j++) {
          sound.put(0,j,i);
          }
          }
          }
        */
        if (!muted) {
            if (put!=NULL) {
                put->renderSound(sound);
            }
        }
        if (saving) {
            saveFrame(sound);
        }

        mutex.post();
        Time::yield();
    }

    void mute(bool muteFlag=true) {
        mutex.wait();
        muted = muteFlag;
        mutex.post();
    }

    void save(bool saveFlag=true) {
        mutex.wait();
        saving = saveFlag;
        mutex.post();
    }

    void saveFrame(Sound& sound) {
        sounds.push_back(sound);
        samples += sound.getSamples();
        channels = sound.getChannels();
        yDebug("  %ld sound frames buffered in memory (%ld samples)\n",
               (long int) sounds.size(),
               (long int) samples);
    }

    bool saveFile(const char *name) {
        mutex.wait();
        saving = false;

        Sound total;
        total.resize(samples,channels);
        long int at = 0;
        while (!sounds.empty()) {
            Sound& tmp = sounds.front();
            for (int i=0; i<channels; i++) {
                for (int j=0; j<tmp.getSamples(); j++) {
                    total.set(tmp.get(j,i),at+j,i);
                }
            }
            total.setFrequency(tmp.getFrequency());
            at += tmp.getSamples();
            sounds.pop_front();
        }
        mutex.post();
        bool ok = write(total,name);
        if (ok) {
            yDebug("Wrote audio to %s\n", name);
        }
        samples = 0;
        channels = 0;
        return ok;
    }

    bool close() {
        port.close();
        mutex.wait(); // onRead never gets called again once it finishes
        return true;
    }
};

int main(int argc, char *argv[]) {
    yarp::os::Network yarp;

    // see if user has supplied audio device
    Property p;
    if (argc>1) {
        p.fromCommand(argc,argv);
    }

    // otherwise default device is "portaudio"
    if (!p.check("device")) {
        p.put("device","portaudio");
        p.put("write",1);
        p.put("delay",1);
    }

    // start the echo service running
    Echo echo;
    echo.open(p);

    // process the keyboard
    bool muted = false;
    bool saving = false;
    bool help = false;
    ConstString fname = "audio_%06d.wav";
    int ct = 0;
    bool done = false;
    while (!done) {
        if (help) {
            yInfo("  Press return to mute/unmute\n");
            yInfo("  Type \"s\" to set start/stop saving audio in memory\n");
            yInfo("  Type \"write filename.wav\" to write saved audio to a file\n");
            yInfo("  Type \"buf NUMBER\" to set buffering delay (default is 0)\n");
            yInfo("  Type \"write\" or \"w\" to write saved audio with same/default name\n");
            yInfo("  Type \"q\" to quit\n");
            yInfo("  Type \"help\" to see this list again\n");
            help = false;
        } else {
            yInfo("Type \"help\" for usage\n");
        }

        ConstString keys = Network::readString();
        Bottle b(keys);
        ConstString cmd = b.get(0).asString();
        if (b.size()==0) {
            muted = !muted;
            echo.mute(muted);
            yInfo("%s\n", muted ? "Muted" : "Audible again");
        } else if (cmd=="help") {
            help = true;
        } else if (cmd=="s") {
            saving = !saving;
            echo.save(saving);
            yInfo("%s\n", saving ? "Saving" : "Stopped saving");
            if (saving) {
                yInfo("  Type \"s\" again to stop saving\n");
            }
        } else if (cmd=="write"||cmd=="w") {
            if (b.size()==2) {
                fname = b.get(1).asString();
            }
            char buf[2560];
            yInfo(buf, fname.c_str(), ct);
            echo.saveFile(buf);
            ct++;
        } else if (cmd=="q"||cmd=="quit") {
            done = true;
        } else if (cmd=="buf"||cmd=="b") {
            padding = b.get(1).asInt();
            yInfo("Buffering at %d\n", padding);
        }
    }

    echo.close();

    return 0;
}
