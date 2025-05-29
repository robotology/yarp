/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <deque>

#include <cstdio>
#include <cmath>
#include <mutex>

#include <yarp/os/Log.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

#include <yarp/sig/SoundFile.h>

#include <yarp/os/impl/Terminal.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::file;
using namespace yarp::dev;

int padding = 0;

class Echo : public TypedReaderCallback<Sound> {
private:
    PolyDriver poly;
    IAudioRender *iAudioplay;
    BufferedPort<Sound> port;
    std::mutex mutex;
    bool muted;
    bool saving;
    std::deque<Sound> sounds;
    size_t samples;
    size_t channels;

public:
    Echo() : mutex() {
        iAudioplay = nullptr;
        port.useCallback(*this);
        port.setStrict();
        muted = false;
        saving = false;
        samples = 0;
        channels = 0;
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
                poly.view(iAudioplay);
                if (iAudioplay == nullptr) {
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

    using TypedReaderCallback<Sound>::onRead;
    void onRead(Sound& sound) override
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
        mutex.lock();
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
            if (iAudioplay != nullptr) {
                iAudioplay->renderSound(sound);
            }
        }
        if (saving) {
            saveFrame(sound);
        }

        mutex.unlock();
        Time::yield();
    }

    void mute(bool muteFlag=true) {
        mutex.lock();
        muted = muteFlag;
        mutex.unlock();
    }

    void save(bool saveFlag=true) {
        mutex.lock();
        saving = saveFlag;
        mutex.unlock();
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
        mutex.lock();
        saving = false;

        Sound total;
        total.resize(samples,channels);
        long int at = 0;
        while (!sounds.empty()) {
            Sound& tmp = sounds.front();
            for (size_t i=0; i<channels; i++) {
                for (size_t j=0; j<tmp.getSamples(); j++) {
                    total.set(tmp.get(j,i),at+j,i);
                }
            }
            total.setFrequency(tmp.getFrequency());
            at += tmp.getSamples();
            sounds.pop_front();
        }
        mutex.unlock();
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
        mutex.lock(); // onRead never gets called again once it finishes
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

    // otherwise default device is "portaudioPlayer"
    if (!p.check("device")) {
        p.put("device","portaudioPlayer");
        p.put("delay",1);
    }

    // start the echo service running
    Echo echo;
    echo.open(p);

    // process the keyboard
    bool muted = false;
    bool saving = false;
    bool help = false;
    std::string fname = "audio_%06d.wav";
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

        std::string keys = yarp::os::impl::Terminal::readString(nullptr);
        Bottle b(keys);
        std::string cmd = b.get(0).asString();
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
            sprintf(buf, fname.c_str(), ct);
            echo.saveFile(buf);
            ct++;
        } else if (cmd=="q"||cmd=="quit") {
            done = true;
        } else if (cmd=="buf"||cmd=="b") {
            padding = b.get(1).asInt32();
            yInfo("Buffering at %d\n", padding);
        }
    }

    echo.close();

    return 0;
}
