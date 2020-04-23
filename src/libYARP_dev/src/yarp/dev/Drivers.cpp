/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/Drivers.h>

#include <yarp/os/Log.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>

#include <vector>
#include <sstream>
#include <iterator>
#include <csignal>

using namespace yarp::os;
using namespace yarp::dev;

class Drivers::Private : public YarpPluginSelector {
public:
    std::vector<DriverCreator *> delegates;

    ~Private() {
        for (auto& delegate : delegates) {
            if (delegate==nullptr) continue;
            delete delegate;
        }
        delegates.clear();
    }

    bool select(Searchable& options) override {
        return options.check("type",Value("none")).asString() == "device";
    }

    std::string toString() {
        std::string s;
        Property done;
        for (auto& delegate : delegates) {
            if (delegate==nullptr) continue;
            std::string name = delegate->getName();
            done.put(name,1);
            std::string wrapper = delegate->getWrapper();
            s += "Device \"";
            s += delegate->getName();
            s += "\"";
            s += ",";
            s += " C++ class ";
            s += delegate->getCode();
            s += ", ";
            if (wrapper=="") {
                s += "has no network wrapper";
            } else if (wrapper!=name) {
                s += "wrapped by \"";
                s += delegate->getWrapper();
                s += "\"";
            } else {
                s += "is a network wrapper.";
            }
            s += "\n";
        }

        scan();
        Bottle lst = getSelectedPlugins();
        for (size_t i=0; i<lst.size(); i++) {
            Value& prop = lst.get(i);
            std::string name = prop.check("name",Value("untitled")).asString();
            if (done.check(name)) continue;

            SharedLibraryFactory lib;
            YarpPluginSettings settings;
            settings.setSelector(*this);
            settings.readFromSearchable(prop,name);
            settings.open(lib);
            std::string location = lib.getName();
            if (location=="") {
              // A wrong library name ends up with empty location
              yWarning("Wrong library name for plugin %s", name.c_str());
              continue;
            }

            std::string cxx = prop.check("cxx",Value("unknown")).asString();
            std::string wrapper = prop.check("wrapper",Value("unknown")).asString();
            s += "Device \"";
            s += name;
            s += "\"";
            s += ",";
            s += " available on request (found in ";
            s += location;
            s += " library)";
            if (cxx!="unknown") {
                s += ", C++ class ";
                s += cxx;
                s += "  ";
            }

            if (wrapper=="") {
                s += "no network wrapper known";  // will never come here since the prop.check fallback is set to unknown few lines above!!!
            } else if (wrapper=="unknown") {
                //s += "network wrapper unknown";
            } else if (wrapper!=name) {
                s += ", wrapped by \"";
                s += wrapper;
                s += "\"";
            } else {
                s += ", is a network wrapper";
            }
            s += ".\n";
        }

        return s;
    }

    void add(DriverCreator *creator) {
        if (creator!=nullptr) {
            delegates.push_back(creator);
        }
    }

    DriverCreator *load(const char *name);

    DriverCreator *find(const char *name) {
        for (auto& delegate : delegates) {
            if (delegate == nullptr) continue;
            std::string s = delegate->toString();
            if (s==name) {
                return delegate;
            }
        }
        return load(name);
    }

    bool remove(const char *name) {
        for (auto& delegate : delegates) {
            if (delegate == nullptr) continue;
            std::string s = delegate->toString();
            if (s==name) {
                delete delegate;
                delegate = nullptr;
            }
        }
        return false;
    }
};

class StubDriver : public DeviceDriver {
private:
    YarpPluginSettings settings;
    YarpPlugin<DeviceDriver> plugin;
    SharedLibraryClass<DeviceDriver> dev;
public:
    StubDriver(const char *dll_name, const char *fn_name, bool verbose = true) {
        settings.setLibraryMethodName(dll_name,fn_name);
        settings.setVerboseMode(verbose);
        init();
    }

    StubDriver(const char *name, bool verbose = true) {
        settings.setPluginName(name);
        settings.setVerboseMode(verbose);
        YarpPluginSelector selector;
        selector.scan();
        if (!settings.setSelector(selector)) {
            return;
        }
        init();
    }

    ~StubDriver() override = default;

    void init() {
        if (plugin.open(settings)) {
            dev.open(*plugin.getFactory());
            settings.setLibraryMethodName(plugin.getFactory()->getName(),
                                          settings.getMethodName());
            settings.setClassInfo(plugin.getFactory()->getClassName(),
                                  plugin.getFactory()->getBaseClassName());
        }
    }

    bool isValid() const {
        return dev.isValid();
    }

    bool open(yarp::os::Searchable& config) override {
        if (!isValid()) return false;
        return dev.getContent().open(config);
    }

    bool close() override {
        if (!isValid()) return false;
        return dev.getContent().close();
    }

    DeviceDriver *getImplementation() override {
        return &dev.getContent();
    }

    std::string getDllName() const {
        return settings.getLibraryName();
    }

    std::string getFnName() const {
        return settings.getMethodName();
    }

    std::string getwrapName() const {
        return settings.getWrapperName();
    }

    std::string getPluginName() const {
        return settings.getPluginName();
    }

    std::string getClassName() const {
        return settings.getClassName();
    }

    std::string getBaseClassName() const {
        return settings.getBaseClassName();
    }
};

Drivers& Drivers::factory()
{
   static Drivers instance;
   return instance;
}

Drivers::Drivers() :
        mPriv(new Private)
{
}

Drivers::~Drivers() {
    delete mPriv;
}

std::string Drivers::toString() const {
    return mPriv->toString();
}

void Drivers::add(DriverCreator *creator) {
    mPriv->add(creator);
}


DriverCreator *Drivers::find(const char *name) {
    return mPriv->find(name);
}

bool Drivers::remove(const char *name) {
    return mPriv->remove(name);
}


DeviceDriver* Drivers::open(yarp::os::Searchable& prop) {
    PolyDriver poly;
    bool result = poly.open(prop);
    if (!result) return nullptr;
    return poly.take();
}

DriverCreator* Drivers::Private::load(const char *name) {
    auto* result = new StubDriver(name,false);
    if (!result->isValid()) {
        delete result;
        result = nullptr;
        return nullptr;
    }
    DriverCreator *creator = new StubDriverCreator(result->getPluginName().c_str(),
                                                   result->getwrapName().c_str(),
                                                   result->getClassName().c_str(),
                                                   result->getDllName().c_str(),
                                                   result->getFnName().c_str());
    add(creator);
    delete result;
    return creator;
}


// helper method for "yarpdev" body
static void toDox(PolyDriver& dd, FILE *os) {
    fprintf(os,"===============================================================\n");
    fprintf(os,"== Options checked by device:\n== \n");

    Bottle order = dd.getOptions();
    for (size_t i=0; i<order.size(); i++) {
        std::string name = order.get(i).toString();
        if (name=="wrapped"||(name.find(".wrapped")!=std::string::npos)) {
            continue;
        }
        std::string desc = dd.getComment(name.c_str());
        Value def = dd.getDefaultValue(name.c_str());
        Value actual = dd.getValue(name.c_str());
        std::string out;
        out += name;
        if (!actual.isNull()) {
            if (actual.toString()!="") {
                out += "=";
                if (actual.toString().length()<40) {
                    out += actual.toString();
                } else {
                    out += "(value too long)";
                }
            }
        }
        if (!def.isNull()) {
            if (def.toString()!="") {
                out += " [";
                if (def.toString().length()<40) {
                    out += def.toString();
                } else {
                    out += "(value too long)";
                }
                out += "]";
            }
        }
        if (desc!="") {
            out += "\n    ";
            out += desc;
        }
        fprintf(os,"%s\n", out.c_str());
    }
    fprintf(os,"==\n");
    fprintf(os,"===============================================================\n");
}


static std::string terminatorKey;
static bool terminated = false;
static void handler (int) {
    Time::useSystemClock();
    static double handleTime = -100;
    static int ct = 0;
    double now = Time::now();
    if (now-handleTime<1) {
        return;
    }
    handleTime = now;
    ct++;
    if (ct>3) {
        yInfo("Aborting...");
        std::exit(1);
    }
    if (terminatorKey!="") {
        yInfo("[try %d of 3] Trying to shut down %s", ct, terminatorKey.c_str());
        terminated = true;
        Terminator::terminateByName(terminatorKey.c_str());
    } else {
        yInfo("Aborting...");
        std::exit(1);
    }
}



// Split with delimiter method.
// See https://stackoverflow.com/questions/236129
// TODO Move somewhere else?
namespace {
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}
} // namespace

int Drivers::yarpdev(int argc, char *argv[]) {

    std::signal(SIGINT, handler);
    std::signal(SIGTERM, handler);

    // get command line options
    ResourceFinder rf;
    rf.configure(argc, argv); // this will process --from FILE if present
    Property options;

    // yarpdev will by default try to pass its thread on to the device.
    // this is because some libraries need configuration and all
    // access methods done in the same thread (e.g. opencv_grabber
    // apparently).
    options.put("single_threaded", 1);

    // interpret command line options as a set of flags
    //options.fromCommand(argc,argv,true,false);
    options.fromString(rf.toString(), false);

    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file",val))
    {
        // FIXME use argv[0]
        yError("*** yarpdev --file is deprecated, please use --from\n");
        yError("*** yarpdev --file will be removed in a future version of YARP\n");

        std::string fname = val->toString();
        options.unput("file");
        yDebug("yarpdev: working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname,false);

        // interpret command line options as a set of flags again
        // (just in case we need to override something)
        options.fromCommand(argc,argv,true,false);
    }

    // check if we want to use nested options (less ambiguous)
    if (options.check("nested",val)||options.check("lispy",val))
    {
        std::string lispy = val->toString();
        yDebug("yarpdev: working with config %s\n", lispy.c_str());
        options.fromString(lispy);
    }

    if (!options.check("device"))
    {
        // no device mentioned - maybe user needs help
        if (options.check("list"))
        {
            yInfo("Here are devices listed for your system:");
            for (auto& s : split(Drivers::factory().toString(), '\n')) {
                yInfo("%s", s.c_str());
            }
        }
        else
        {
            yInfo("Welcome to yarpdev, a program to create YARP devices\n");
            yInfo("To see the devices available, try:\n");
            yInfo("   yarpdev --list\n");
            yInfo("To create a device whose name you know, call yarpdev like this:\n");
            yInfo("   yarpdev --device DEVICENAME --OPTION VALUE ...\n");
            yInfo("   For example:\n");
            yInfo("   yarpdev --device test_grabber --width 32 --height 16 --name /grabber\n");
            yInfo("You can always move options to a configuration file:\n");
            yInfo("   yarpdev [--device DEVICENAME] --from CONFIG_FILENAME\n");
            yInfo("If you have problems, you can add the \"verbose\" flag to get more information\n");
            yInfo("   yarpdev --verbose --device ffmpeg_grabber\n");
            if (options.check ("from"))
            {
                yError("Unable to find --device option in file %s. Closing.", options.find("from").asString().c_str());
            }
            else
            {
                yWarning("--device option not specified. Closing.");
            }
        }
        return 0;
    }

    // ask for a wrapped, remotable device rather than raw device
    options.put("wrapped","1");

    //YarpDevMonitor monitor;
    bool verbose = false;
    if (options.check("verbose")) {
        verbose = true;
        //options.setMonitor(&monitor,"top-level");
    }

    // we now need network
    bool ret=Network::checkNetwork();
    if (!ret)
    {
        yError("YARP network not available, check if yarp server is reachable\n");
        return -1;
    }

    //
    // yarpdev initializes the clock only before starting to do real thing.
    // This way yarpdev --lish/help will not be affected by network clock.
    //
    // Shall other devices be affected by network clock ??
    // Hereafter the device may need to use the SystemClock or the NetworkClock
    // depending by the device, a real or a fake / simulated one.
    // Using the YARP_CLOCK_DEFAULT the behaviour will be determined by the
    // environment variable.
    //
    yarp::os::NetworkBase::yarpClockInit(yarp::os::YARP_CLOCK_DEFAULT);

    PolyDriver dd(options);
    if (verbose) {
        toDox(dd,stdout);
    }
    if (!dd.isValid()) {
        yError("yarpdev: ***ERROR*** device not available.\n");
        if (argc==1)
        {
            yInfo("Here are the known devices:\n");
            yInfo("%s", Drivers::factory().toString().c_str());
        }
        else
        {
            yInfo("Suggestions:\n");
            yInfo("+ Do \"yarpdev --list\" to see list of supported devices.\n");
            if (!options.check("verbose"))
            {
                yInfo("+ Or append \"--verbose\" option to get more information.\n");
            }
        }
        return 1;
    }

    Terminee *terminee = nullptr;
    if (dd.isValid()) {
        Value *v;
        std::string name;
        if (options.check("name", v)) {
            name = v->toString();
        } else if (options.check("device", v)) {
            if (v->isString()) {
                auto device_name = v->toString();
                name = dd.getDefaultValue((device_name + ".name").c_str()).toString();
                if (name.empty()) {
                    auto options = dd.getOptions();
                    for (size_t i = 0; i < options.size(); ++i) {
                        auto opt = options.get(i).toString();
                        if (opt.length() > 5 && opt.compare(opt.length() - 5, 5, ".name") == 0) { // C++20 opt.ends_with(".name")
                            yWarning("%s", opt.c_str());
                            name = dd.getDefaultValue(opt.c_str()).toString();
                            break;
                        }
                    }
                }
                if (name.empty()) {
                    name = v->toString();
                }
            }
        } else {
            name = "/yarpdev";
        }
        std::string s = name + "/quit";

        if (s.find('=') == std::string::npos &&
            s.find('@') == std::string::npos) {
            terminee = new Terminee(s.c_str());
            terminatorKey = s;
            if (terminee == nullptr) {
                yError("Can't allocate terminator port\n");
                terminatorKey = "";
                dd.close();
                return 1;
            }
            if (!terminee->isOk()) {
                yError("Failed to create terminator port\n");
                terminatorKey = "";
                delete terminee;
                terminee = nullptr;
                dd.close();
                return 1;
            }
        }
    }

    double dnow = 3;
    double startTime = Time::now()-dnow;
    IService *service = nullptr;
    dd.view(service);
    if (service!=nullptr) {
        bool backgrounded = service->startService();
        if (backgrounded) {
            // we don't need to poll this, so forget about the
            // service interface
            yDebug("yarpdev: service backgrounded\n");
            service = nullptr;
        }
    }
    while (dd.isValid() && !(terminated||(terminee&&terminee->mustQuit()))) {
        if (service!=nullptr) {
            double now = Time::now();
            if (now-startTime>dnow) {
                yInfo("device active...");
                startTime += dnow;
            }
            // we requested single threading, so need to
            // give the device its chance
            if(!service->updateService()) {
                if(!service->stopService()) {
                    yWarning("Error while stopping device");
                }
                terminated = true;
            }
        } else {
            // we don't need to do anything
            yInfo("device active in background...");
            SystemClock::delaySystem(dnow);
        }
    }

    if (terminee) {
        delete terminee;
        terminee = nullptr;
    }
    dd.close();

    yInfo("yarpdev is finished.");

    return 0;
}

DeviceDriver *StubDriverCreator::create() const {
    //yDebug("Creating %s from %s\n", desc.c_str(), libname.c_str());
    auto* result = new StubDriver(libname.c_str(),fnname.c_str(),false);
    if (result==nullptr) return result;
    if (!result->isValid()) {
        delete result;
        result = nullptr;
        return nullptr;
    }
    //yDebug("Created %s from %s\n", desc.c_str(), libname.c_str());
    return result;
}
