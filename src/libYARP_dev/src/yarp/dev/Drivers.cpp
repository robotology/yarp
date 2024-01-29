/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/Drivers.h>

#include <yarp/conf/string.h>

#include <yarp/os/Log.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Terminator.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/IDeviceDriverParams.h>

#include <vector>
#include <sstream>
#include <iterator>
#include <csignal>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(DRIVERS, "yarp.dev.Drivers")
}

class Drivers::Private : public YarpPluginSelector {
public:
    std::vector<DriverCreator *> delegates;

    ~Private() override {
        for (auto& delegate : delegates) {
            if (delegate==nullptr) {
                continue;
            }
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
            if (delegate==nullptr) {
                continue;
            }
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
            if (wrapper.empty()) {
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
            if (done.check(name)) {
                continue;
            }

            SharedLibraryFactory lib;
            YarpPluginSettings settings;
            settings.setSelector(*this);
            settings.readFromSearchable(prop,name);
            settings.open(lib);
            std::string location = lib.getName();
            if (location.empty()) {
              // A wrong library name ends up with empty location
              yCWarning(DRIVERS, "Wrong library name for plugin %s", name.c_str());
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

            if (wrapper.empty()) {
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
            if (delegate == nullptr) {
                continue;
            }
            std::string s = delegate->toString();
            if (s==name) {
                return delegate;
            }
        }
        return load(name);
    }

    bool remove(const char *name) {
        for (auto& delegate : delegates) {
            if (delegate == nullptr) {
                continue;
            }
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
    StubDriver(const char *dll_name, const char *fn_name) {
        settings.setLibraryMethodName(dll_name,fn_name);
        init();
    }

    StubDriver(const char *name) {
        settings.setPluginName(name);
        YarpPluginSelector selector;
        selector.scan();
        if (!settings.setSelector(selector)) {
            return;
        }
        init();
    }

    virtual ~StubDriver() = default;

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
        if (!isValid()) {
            return false;
        }
        return dev.getContent().open(config);
    }

    bool close() override {
        if (!isValid()) {
            return false;
        }
        return dev.getContent().close();
    }

    void setId(const std::string& id) override
    {
        if (!isValid()) {
            return;
        }
        dev.getContent().setId(id);
    }

    std::string id() const override
    {
        if (!isValid()) {
            return "StubDriver";
        }
        return dev.getContent().id();
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
    if (!result) {
        return nullptr;
    }
    return poly.take();
}

DriverCreator* Drivers::Private::load(const char *name) {
    auto* result = new StubDriver(name);
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

static std::string terminatorKey;
static bool terminated = false;
static void handler (int)
{
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
        yCInfo(DRIVERS, "Aborting...");
        std::exit(1);
    }
    if (!terminatorKey.empty()) {
        yCInfo(DRIVERS, "[try %d of 3] Trying to shut down %s", ct, terminatorKey.c_str());
        terminated = true;
        Terminator::terminateByName(terminatorKey.c_str());
    } else {
        yCInfo(DRIVERS, "Aborting...");
        std::exit(1);
    }
}


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
    if (options.check("file",val)) {
        yCError(DRIVERS, "*** yarpdev --file is deprecated, please use --from");
        return 0;
    }

    // print the  full list of devices, if requested
    if (options.check("list")) {
        yCInfo(DRIVERS, "Here are devices listed for your system:");
        for (const auto& s : yarp::conf::string::split(Drivers::factory().toString(), '\n')) {
            yCInfo(DRIVERS, "%s", s.c_str());
        }
        return 0;
    }

    // check if we want to use nested options (less ambiguous)
    if (options.check("nested", val) || options.check("lispy", val)) {
        std::string lispy = val->toString();
        yCDebug(DRIVERS, "yarpdev: working with config %s", lispy.c_str());
        options.fromString(lispy);
    }

    // no device mentioned, neither in the option, nor in configuration file - maybe user needs help
    if (!options.check("device") && !options.check("from"))
    {
        yCInfo(DRIVERS, "Welcome to yarpdev, a program to create YARP devices");
        yCInfo(DRIVERS, "To see the devices available, try:");
        yCInfo(DRIVERS, "   yarpdev --list");
        yCInfo(DRIVERS, "To create a device whose name you know, call yarpdev like this:");
        yCInfo(DRIVERS, "   yarpdev --device DEVICENAME --OPTION VALUE ...");
        yCInfo(DRIVERS, "   For example:");
        yCInfo(DRIVERS, "   yarpdev --device fakeFrameGrabber --width 32 --height 16 --name /grabber");
        yCInfo(DRIVERS, "If the device supports it, you can check its configuration parameters using the --help option");
        yCInfo(DRIVERS, "You can always move options to a configuration file:");
        yCInfo(DRIVERS, "   yarpdev [--device DEVICENAME] --from CONFIG_FILENAME");
        yCError(DRIVERS,"Device name not specified");
        return 0;
    }

    // ask for a wrapped, remotable device rather than raw device
    options.put("wrapped","1");

    //YarpDevMonitor monitor;
    if (options.check("verbose")) {
        yCWarning(DRIVERS, "The verbose option is deprecated.");
    }

    // we now need network
    bool ret=Network::checkNetwork();
    if (!ret) {
        yCError(DRIVERS, "YARP network not available, check if yarp server is reachable");
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

    // Finally open the device driver
    PolyDriver dd(options);
    std::string id = dd.id();

    //if the device.open() failed...
    if (!dd.isValid()) {
        yCIError(DRIVERS, id, "yarpdev: ***ERROR*** device not available.");
        if (argc==1)
        {
            yCIInfo(DRIVERS, id, "Here are the known devices:");
            yCIInfo(DRIVERS, id, "%s", Drivers::factory().toString().c_str());
        }
        else
        {
            yCIInfo(DRIVERS, id, "Suggestions:");
            yCIInfo(DRIVERS, id, "+ Do \"yarpdev --list\" to see list of supported devices.");
        }
        return 1;
    }

    Terminee *terminee = nullptr;
    if (dd.isValid())
    {
        Value *v;
        std::string name;
        if (options.check("name", v))
        {
            name = v->toString();
        }
        if (name.empty() && options.check("device", v))
        {
            //get the name of the device from the polydriver
            //TO BE IMPLEMENTED. FOR REFERENCE ONLY:
            //name = dd.getDefaultValue((device_name + ".name").c_str()).toString();
        }
        if (name.empty())
        {
            name = "/yarpdev";
        }
        std::string s = name + "/quit";

        if (s.find('=') == std::string::npos &&
            s.find('@') == std::string::npos) {
            terminee = new Terminee(s.c_str());
            terminatorKey = s;
            if (terminee == nullptr) {
                yCIError(DRIVERS, id, "Can't allocate terminator port");
                terminatorKey = "";
                dd.close();
                return 1;
            }
            if (!terminee->isOk()) {
                yCIError(DRIVERS, id, "Failed to create terminator port");
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
            yCIDebug(DRIVERS, id, "yarpdev: service backgrounded");
            service = nullptr;
        }
    }
    while (dd.isValid() && !(terminated||(terminee&&terminee->mustQuit()))) {
        if (service!=nullptr) {
            double now = Time::now();
            if (now-startTime>dnow) {
                yCIInfo(DRIVERS, id, "device active...");
                startTime += dnow;
            }
            // we requested single threading, so need to
            // give the device its chance
            if(!service->updateService()) {
                if(!service->stopService()) {
                    yCIWarning(DRIVERS, id, "Error while stopping device");
                }
                terminated = true;
            }
        } else {
            // we don't need to do anything
            yCIInfo(DRIVERS, id, "device active in background...");
            SystemClock::delaySystem(dnow);
        }
    }

    if (terminee) {
        delete terminee;
        terminee = nullptr;
    }
    dd.close();

    yCIInfo(DRIVERS, id, "yarpdev is finished.");

    return 0;
}

DeviceDriver *StubDriverCreator::create() const {
    yCTrace(DRIVERS, "Creating %s from %s", desc.c_str(), libname.c_str());
    auto* result = new StubDriver(libname.c_str(),fnname.c_str());
    if (result==nullptr) {
        return result;
    }
    if (!result->isValid()) {
        delete result;
        result = nullptr;
        return nullptr;
    }
    yCTrace(DRIVERS, "Created %s from %s", desc.c_str(), libname.c_str());
    return result;
}
