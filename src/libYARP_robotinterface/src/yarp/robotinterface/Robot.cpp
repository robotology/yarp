/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/Robot.h>

#include <yarp/robotinterface/Action.h>
#include <yarp/robotinterface/Device.h>
#include <yarp/robotinterface/Param.h>

#include <yarp/os/LogStream.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>

#include <yarp/dev/IDeviceDriverParams.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_set>

#include <yarp/dev/IRobotDescription.h>

namespace {
YARP_LOG_COMPONENT(YRI_ROBOT, "yarp.yri.Robot")
}

yarp::os::LogStream operator<<(yarp::os::LogStream dbg, const yarp::robotinterface::Robot& t)
{
    dbg << "(name = \"" << t.name() << "\"";
    if (!t.params().empty()) {
        dbg << ", params = [";
        dbg << t.params();
        dbg << "]";
    }
    if (!t.devices().empty()) {
        dbg << ", devices = [";
        dbg << t.devices();
        dbg << "]";
    }
    dbg << ")";
    return dbg;
}


class yarp::robotinterface::Robot::Private
{
public:
    Private(Robot* /*parent*/)
    {
    }

    // return true if a device with the given name exists
    bool hasDevice(const std::string& name) const;

    // return the device with the given name or <fatal error> if not found
    Device* findDevice(const std::string& name);

    //Print a debug message with a list of devices
    void PrintDevicesIncludingExternal() const;

    // return true if a device with the given name exists
    // considering also the provided external devices
    bool hasDeviceIncludingExternal(const std::string& name) const;

    // return the device with the given name or nullptr if not found,
    // considering also the provided external devices
    yarp::dev::PolyDriverDescriptor findDeviceIncludingExternal(const std::string& name);

    // check if there is no external devices that has the same name of an internal device
    // return true if there is a conflict, false otherwise
    bool checkForNamingConflictsInExternalDevices(const yarp::dev::PolyDriverList& newExternalDevicesList);

    // open all the devices and return true if all the open calls were successful
    bool openDevices();

    // close all the devices and return true if all the close calls were successful
    bool closeDevices();

    // return a vector of levels that have actions in the requested phase
    std::vector<unsigned int> getLevels(ActionPhase phase) const;

    // return a vector of actions for that phase and that level
    std::vector<std::pair<Device, Action>> getActions(ActionPhase phase, unsigned int level) const;


    // run configure action on one device
    bool configure(const Device& device, const ParamList& params);

    // run calibrate action on one device
    bool calibrate(const Device& device, const ParamList& params);

    // run attach action on one device
    bool attach(const Device& device, const ParamList& params);

    // run abort action on one device
    bool abort(const Device& device, const ParamList& params);

    // run detach action on one device
    bool detach(const Device& device, const ParamList& params);

    // run park action on one device
    bool park(const Device& device, const ParamList& params);

    // run custom action on one device
    bool custom(const Device& device, const ParamList& params);

    std::string name;
    unsigned int build {0};
    std::string portprefix;
    ParamList params;
    DeviceList devices;
    yarp::dev::PolyDriverList externalDevices;
    yarp::robotinterface::ActionPhase currentPhase {ActionPhaseUnknown};
    unsigned int currentLevel {0};
    bool dryrun {false};
    bool reverseShutdownActionOrder {false};
    yarp::dev::PolyDriver m_ddstorage;
    std::string yriDescriptionStorageName = "yriDescriptionStorage";
    yarp::dev::IRobotDescription* m_istorage = nullptr;

}; // class yarp::robotinterface::Robot::Private

bool yarp::robotinterface::Robot::Private::hasDevice(const std::string& name) const
{
    for (const auto& device : devices) {
        if (name == device.name()) {
            return true;
        }
    }
    return false;
}

yarp::robotinterface::Device* yarp::robotinterface::Robot::Private::findDevice(const std::string& name)
{
    for (auto& device : devices) {
        if (name == device.name()) {
            return &device;
        }
    }
    return nullptr;
}

void yarp::robotinterface::Robot::Private::PrintDevicesIncludingExternal() const
{
    for (int i = 0; i < externalDevices.size(); i++) {
        const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
        yCInfo(YRI_ROBOT) << "- " << externalDevice->key;
    }
}

bool yarp::robotinterface::Robot::Private::hasDeviceIncludingExternal(const std::string& name) const
{
    if (hasDevice(name)) {
        return true;
    } else {
        for (int i = 0; i < externalDevices.size(); i++) {
            const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
            if (name == externalDevice->key) {
                return true;
            }
        }
    }
    return false;
}

yarp::dev::PolyDriverDescriptor
yarp::robotinterface::Robot::Private::findDeviceIncludingExternal(const std::string& name)
{
    yarp::dev::PolyDriverDescriptor deviceFound;
    deviceFound.poly = nullptr;
    deviceFound.key = "";

    for (auto& device : devices) {
        if (name == device.name()) {
            deviceFound.poly = device.driver();
            deviceFound.key = device.name();
            return deviceFound;
        }
    }

    for (int i = 0; i < externalDevices.size(); i++) {
        const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
        if (name == externalDevice->key) {
            deviceFound = *externalDevice;
        }
    }

    return deviceFound;
}

bool yarp::robotinterface::Robot::Private::checkForNamingConflictsInExternalDevices(const yarp::dev::PolyDriverList& externalDevicesList)
{
    std::unordered_set<std::string> externalDevicesNames;

    for (int i = 0; i < externalDevicesList.size(); i++) {
        const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevicesList[i];
        externalDevicesNames.insert(externalDevice->key);
    }

    for (auto& device : devices) {
        if (externalDevicesNames.find(device.name()) != externalDevicesNames.end()) {
            yCError(YRI_ROBOT) << "Device name " << device.name() << " is used for both an internal and external device.";
            return true;
        }
    }

    return false;
}


bool yarp::robotinterface::Robot::Private::openDevices()
{
    yarp::os::Property pcfg;
    pcfg.put("device", "robotDescriptionStorage");
    pcfg.put("name", "yriDescriptionStorage");
    m_ddstorage.open(pcfg);
    m_ddstorage.view(m_istorage);

    bool ret = true;
    for (auto& device : devices) {
        yCInfo(YRI_ROBOT) << "Opening device" << device.name() << "with parameters" << device.params();

        if (dryrun) {
            continue;
        }

        if (!device.open()) {
            yCWarning(YRI_ROBOT) << "Cannot open device" << device.name();
            ret = false;
        }
        else
        {
            if (m_istorage)
            {
                std::vector<std::string> stp;
                std::string scfg;
                yarp::dev::PolyDriver* pddrv = device.driver();
                yarp::dev::IDeviceDriverParams* dparams = nullptr;
                if (pddrv)
                {
                    pddrv->view(dparams);
                    if (dparams)
                    {
                        stp = dparams->getListOfParams();
                        scfg = dparams->getConfiguration();
                    }
                    else
                    {
                        yCWarning(YRI_ROBOT) << "Device" << device.name() << "does not derive from IDeviceDriverParams.";
                    }
                }
                if (!pddrv || scfg.empty())
                {
                    yCWarning(YRI_ROBOT) << "Unable to get device" << device.name() << "configuration. yarprobotinterface will continue, but some features for inspecting the device parameters will be disabled.";
                    yCDebug(YRI_ROBOT) << "It is recommended that devices used by yarprobinterface implement the `yarp::dev::IDeviceDriverParams` interface.";
                    yCDebug(YRI_ROBOT) << "See yarprobotinterface documentation page.";
                }
                yarp::dev::DeviceDescription devdesc;
                devdesc.device_name = device.name();
                devdesc.device_type = device.type();
                devdesc.device_configuration = scfg;
                yarp::dev::ReturnValue ret = m_istorage->registerDevice(devdesc);
                if (!ret)
                {
                    yCError(YRI_ROBOT) << "Unable to register device" << device.name() << "in robotDescriptionStorage";
                }
            }
        }
    }
    if (ret)
    {
        if (m_istorage)
        {
            std::vector<yarp::dev::DeviceDescription> ll;
            m_istorage->getAllDevices(ll);
            std::ostringstream oss;
            for (auto& it_device : ll)
            {
                oss << "- name:" << it_device.device_name << ", type:" << it_device.device_type << "\n";
            }
            yCInfo(YRI_ROBOT) << "List of opened devices:\n"
                              << oss.str() << "End of list";
            ;
        }
    }
    else
    {
        yCWarning(YRI_ROBOT) << "There was some problem opening one or more devices. Please check the log and your configuration";
    }

    return ret;
}

bool yarp::robotinterface::Robot::Private::closeDevices()
{
    bool ret = true;
    for (auto it = devices.rbegin(); it != devices.rend(); ++it) {
        yarp::robotinterface::Device& device = *it;

        yCInfo(YRI_ROBOT) << "Closing device" << device.name();

        if (dryrun) {
            continue;
        }

        // yCDebug(YRI_ROBOT) << device;

        if (!device.close()) {
            yCWarning(YRI_ROBOT) << "Cannot close device" << device.name();
            ret = false;
        }
    }
    if (ret) {
        yCInfo(YRI_ROBOT) << "All devices closed.";
    } else {
        yCWarning(YRI_ROBOT) << "There was some problem closing one or more devices. Please check the log and your configuration";
    }

    if (m_ddstorage.isValid())
    {
        m_ddstorage.close();
    }

    return ret;
}

std::vector<unsigned int> yarp::robotinterface::Robot::Private::getLevels(yarp::robotinterface::ActionPhase phase) const
{
    std::vector<unsigned int> levels;
    for (const auto& device : devices) {
        if (device.actions().empty()) {
            continue;
        }

        for (const auto& action : device.actions()) {
            if (action.phase() == phase) {
                levels.push_back(action.level());
            }
        }
    }

    std::sort(levels.begin(), levels.end());
    auto it = std::unique(levels.begin(), levels.end());
    levels.resize(it - levels.begin());

    return levels;
}


std::vector<std::pair<yarp::robotinterface::Device, yarp::robotinterface::Action>> yarp::robotinterface::Robot::Private::getActions(yarp::robotinterface::ActionPhase phase, unsigned int level) const
{
    std::vector<std::pair<yarp::robotinterface::Device, yarp::robotinterface::Action>> actions;
    for (const auto& device : devices) {
        if (device.actions().empty()) {
            continue;
        }

        for (const auto& action : device.actions()) {
            if (action.phase() == phase && action.level() == level) {
                actions.emplace_back(device, action);
            }
        }
    }
    return actions;
}


bool yarp::robotinterface::Robot::Private::configure(const yarp::robotinterface::Device& device, const yarp::robotinterface::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("configure action")
    return true;
}

bool yarp::robotinterface::Robot::Private::calibrate(const yarp::robotinterface::Device& device,
                                                     const yarp::robotinterface::ParamList& params)
{
    if (!yarp::robotinterface::hasParam(params, "target")) {
        yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeCalibrate) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = yarp::robotinterface::findParam(params, "target");

    if (!hasDeviceIncludingExternal(targetDeviceName)) {
        yCError(YRI_ROBOT) << "Target device" << targetDeviceName << "does not exist.";
        yCError(YRI_ROBOT) << "Available target devices are:";
        PrintDevicesIncludingExternal();
        return false;
    }

    if (dryrun) {
        return true;
    }

    yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

    return device.calibrate(targetDevice);
}

bool yarp::robotinterface::Robot::Private::attach(const yarp::robotinterface::Device& device,
                                                  const yarp::robotinterface::ParamList& params)
{
    if (device.type() == "robotDescription_nws_yarp")
    {
        std::string storageName = yarp::robotinterface::findParam(params, "device");
        yarp::dev::PolyDriverList drv_list;
        drv_list.push(&m_ddstorage, yriDescriptionStorageName.c_str());
        bool b= device.attach(drv_list);
        if (!b)
        {
            yCError(YRI_ROBOT) << "cannot attach robotDescription_nws_yarp to yriDescriptionStorage";
        }
        return true;
    }

    int check = 0;
    if (yarp::robotinterface::hasParam(params, "network")) {
        check++;
    }
    if (yarp::robotinterface::hasParam(params, "networks")) {
        check++;
    }
    if (yarp::robotinterface::hasParam(params, "all")) {
        check++;
    }

    if (check > 1) {
        yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" : you can have only one option: "network" , "networks" or "all" )";
        return false;
    }

    yarp::dev::PolyDriverList drivers;

    if (yarp::robotinterface::hasParam(params, "device")) {
        std::string targetDeviceName = yarp::robotinterface::findParam(params, "device");

        std::string targetNetwork = "...";
        if (yarp::robotinterface::hasParam(params, "network")) {
            targetNetwork = yarp::robotinterface::findParam(params, "network");
        }

        if (!hasDeviceIncludingExternal(targetDeviceName)) {
            yCError(YRI_ROBOT) << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
            yCError(YRI_ROBOT) << "Available target devices are:";
            PrintDevicesIncludingExternal();
            return false;
        }

        if (!dryrun) {
            yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

            // yCDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
            drivers.push(targetDevice.poly, targetNetwork.c_str());
        }

    } else if (yarp::robotinterface::hasParam(params, "all")) {
        if (!dryrun) {
            for (auto& device : devices) {
                drivers.push(device.driver(), "all");
            }

            for (int i = 0; i < externalDevices.size(); i++) {
                const yarp::dev::PolyDriverDescriptor* externalDevice = externalDevices[i];
                drivers.push(externalDevice->poly, "all");
            }
        }
    } else if (yarp::robotinterface::hasParam(params, "networks")) {
        yarp::os::Value v;
        v.fromString(yarp::robotinterface::findParam(params, "networks").c_str());
        yarp::os::Bottle& targetNetworks = *(v.asList());

        for (size_t i = 0; i < targetNetworks.size(); ++i) {
            std::string targetNetwork = targetNetworks.get(i).toString();

            if (!yarp::robotinterface::hasParam(params, targetNetwork)) {
                yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" requires one parameter per network. \"" << targetNetwork << "\" parameter is missing.";
                return false;
            }
            std::string targetDeviceName = yarp::robotinterface::findParam(params, targetNetwork);
            if (!hasDeviceIncludingExternal(targetDeviceName)) {
                yCError(YRI_ROBOT) << "Target device" << targetDeviceName << "(network =" << targetNetwork << ") does not exist.";
                yCError(YRI_ROBOT) << "Available target devices are:";
                PrintDevicesIncludingExternal();
                return false;
            }

            if (!dryrun) {
                yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

                // yCDebug() << "Attach device" << device.name() << "to" << targetDevice.name() << "as" << targetNetwork;
                drivers.push(targetDevice.poly, targetNetwork.c_str());
            }
        }
    } else {
        yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeAttach) << R"(" requires either "network" or "networks" parameter)";
        return false;
    }

    if (dryrun) {
        return true;
    }

    if (!drivers.size()) {
        yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeAttach) << "\" couldn't find any device.";
        return false;
    }

    return device.attach(drivers);
}

bool yarp::robotinterface::Robot::Private::abort(const yarp::robotinterface::Device& device, const yarp::robotinterface::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("abort action")
    return true;
}


bool yarp::robotinterface::Robot::Private::detach(const yarp::robotinterface::Device& device, const yarp::robotinterface::ParamList& params)
{
    if (!params.empty()) {
        yCWarning(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypeDetach) << "\" cannot have any parameter. Ignoring them.";
    }

    if (dryrun) {
        return true;
    }

    return device.detach();
}

bool yarp::robotinterface::Robot::Private::park(const yarp::robotinterface::Device& device,
                                                              const yarp::robotinterface::ParamList& params)
{
    if (!yarp::robotinterface::hasParam(params, "target")) {
        yCError(YRI_ROBOT) << "Action \"" << ActionTypeToString(ActionTypePark) << R"(" requires "target" parameter)";
        return false;
    }
    std::string targetDeviceName = yarp::robotinterface::findParam(params, "target");

    if (!hasDeviceIncludingExternal(targetDeviceName)) {
        yCError(YRI_ROBOT) << "Target device" << targetDeviceName << "does not exist.";
        yCError(YRI_ROBOT) << "Available target devices are:";
        PrintDevicesIncludingExternal();
        return false;
    }

    if (dryrun) {
        return true;
    }

    yarp::dev::PolyDriverDescriptor targetDevice = findDeviceIncludingExternal(targetDeviceName);

    return device.park(targetDevice);
}

bool yarp::robotinterface::Robot::Private::custom(const yarp::robotinterface::Device& device, const yarp::robotinterface::ParamList& params)
{
    YARP_FIXME_NOTIMPLEMENTED("custom action")
    return true;
}

yarp::robotinterface::Robot::Robot() :
        mPriv(new Private(this))
{
}

yarp::robotinterface::Robot::Robot(const std::string& name, const yarp::robotinterface::DeviceList& devices) :
        mPriv(new Private(this))
{
    mPriv->name = name;
    mPriv->devices = devices;
}

yarp::robotinterface::Robot::Robot(const yarp::robotinterface::Robot& other) :
        mPriv(new Private(this))
{
    mPriv->name = other.mPriv->name;
    mPriv->build = other.mPriv->build;
    mPriv->portprefix = other.mPriv->portprefix;
    mPriv->currentPhase = other.mPriv->currentPhase;
    mPriv->currentLevel = other.mPriv->currentLevel;
    mPriv->dryrun = other.mPriv->dryrun;
    mPriv->reverseShutdownActionOrder = other.mPriv->reverseShutdownActionOrder;
    mPriv->devices = other.mPriv->devices;
    mPriv->params = other.mPriv->params;
}

yarp::robotinterface::Robot& yarp::robotinterface::Robot::operator=(const yarp::robotinterface::Robot& other)
{
    if (&other != this) {
        mPriv->name = other.mPriv->name;
        mPriv->build = other.mPriv->build;
        mPriv->portprefix = other.mPriv->portprefix;
        mPriv->currentPhase = other.mPriv->currentPhase;
        mPriv->currentLevel = other.mPriv->currentLevel;
        mPriv->dryrun = other.mPriv->dryrun;
        mPriv->reverseShutdownActionOrder = other.mPriv->reverseShutdownActionOrder;

        mPriv->devices.clear();
        mPriv->devices = other.mPriv->devices;

        mPriv->params.clear();
        mPriv->params = other.mPriv->params;
    }

    return *this;
}

yarp::robotinterface::Robot::~Robot()
{
    delete mPriv;
}

std::string& yarp::robotinterface::Robot::name()
{
    return mPriv->name;
}

unsigned int& yarp::robotinterface::Robot::build()
{
    return mPriv->build;
}

std::string& yarp::robotinterface::Robot::portprefix()
{
    return mPriv->portprefix;
}

void yarp::robotinterface::Robot::setVerbose(bool verbose)
{
    for (auto& device : devices()) {
        ParamList& params = device.params();
        // Do not override "verbose" param if explicitly set in the xml
        if (verbose && !yarp::robotinterface::hasParam(params, "verbose")) {
            device.params().push_back(Param("verbose", "1"));
        }
    }
}

void yarp::robotinterface::Robot::setAllowDeprecatedDevices(bool allowDeprecatedDevices)
{
    for (auto& device : devices()) {
        ParamList& params = device.params();
        // Do not override "allow-deprecated-devices" param if explicitly set in the xml
        if (allowDeprecatedDevices && !yarp::robotinterface::hasParam(params, "allow-deprecated-devices")) {
            device.params().push_back(Param("allow-deprecated-devices", "1"));
        }
    }
}

void yarp::robotinterface::Robot::setDryRun(bool dryrun)
{
    mPriv->dryrun = dryrun;
}

void yarp::robotinterface::Robot::setReverseShutdownActionOrder(bool reverseShutdownActionOrder)
{
    mPriv->reverseShutdownActionOrder = reverseShutdownActionOrder;
}

yarp::robotinterface::ParamList& yarp::robotinterface::Robot::params()
{
    return mPriv->params;
}

yarp::robotinterface::DeviceList& yarp::robotinterface::Robot::devices()
{
    return mPriv->devices;
}

yarp::robotinterface::Device& yarp::robotinterface::Robot::device(const std::string& name)
{
    return *mPriv->findDevice(name);
}

const std::string& yarp::robotinterface::Robot::name() const
{
    return mPriv->name;
}

const unsigned int& yarp::robotinterface::Robot::build() const
{
    return mPriv->build;
}

const std::string& yarp::robotinterface::Robot::portprefix() const
{
    return mPriv->portprefix;
}

const yarp::robotinterface::ParamList& yarp::robotinterface::Robot::params() const
{
    return mPriv->params;
}

const yarp::robotinterface::DeviceList& yarp::robotinterface::Robot::devices() const
{
    return mPriv->devices;
}

bool yarp::robotinterface::Robot::hasDevice(const std::string& name) const
{
    return mPriv->hasDevice(name);
}

const yarp::robotinterface::Device& yarp::robotinterface::Robot::device(const std::string& name) const
{
    return *mPriv->findDevice(name);
}

void yarp::robotinterface::Robot::interrupt()
{
    yCInfo(YRI_ROBOT) << "Interrupt received. Stopping all running threads.";

    // If we received an interrupt we send a stop signal to all threads
    // from previous phases
    for (auto& device : devices()) {
        device.stopThreads();
    }
}

bool yarp::robotinterface::Robot::setExternalDevices(const yarp::dev::PolyDriverList& list)
{
    bool nameConflict = mPriv->checkForNamingConflictsInExternalDevices(list);
    if (nameConflict) {
        return false;
    }

    mPriv->externalDevices = list;
    return true;
}

bool yarp::robotinterface::Robot::enterPhase(yarp::robotinterface::ActionPhase phase)
{
    yCInfo(YRI_ROBOT) << ActionPhaseToString(phase) << "phase starting...";

    mPriv->currentPhase = phase;
    mPriv->currentLevel = 0;

    // Open all devices
    if (phase == ActionPhaseStartup) {
        if (!mPriv->openDevices()) {
            yCError(YRI_ROBOT) << "One or more devices failed opening... see previous log messages for more info";
            if (!mPriv->closeDevices()) {
                yCError(YRI_ROBOT) << "One or more devices failed closing";
            }
            return false;
        }
    }

    // run phase does not accept actions
    if (phase == ActionPhaseRun) {
        if (mPriv->getLevels(phase).size() != 0) {
            yCWarning(YRI_ROBOT) << "Phase" << ActionPhaseToString(phase) << "does not accept actions. Skipping all actions for this phase";
        }
        return true;
    }

    // Before starting any action we ensure that there are no other
    // threads running from previous phases.
    // In interrupt 2 and 3 and this is called by the interrupt callback,
    // and therefore main thread will be blocked and join will never
    // return. Therefore, since we want to start the abort actions we
    // skip this check.
    if (phase != ActionPhaseInterrupt2 && phase != ActionPhaseInterrupt3) {
        for (auto& device : devices()) {
            device.joinThreads();
        }
    }

    std::vector<unsigned int> levels = mPriv->getLevels(phase);
    if (mPriv->reverseShutdownActionOrder && (phase == ActionPhaseShutdown ||
                                              phase == ActionPhaseInterrupt1 ||
                                              phase == ActionPhaseInterrupt2 ||
                                              phase == ActionPhaseInterrupt3)) {
        std::reverse(levels.begin(), levels.end());
    }

    bool ret = true;
    for (unsigned int level : levels) {
        // for each level
        yCInfo(YRI_ROBOT) << "Entering action level" << level << "of phase" << ActionPhaseToString(phase);
        mPriv->currentLevel = level;

        // If current phase was changed by some other thread, we should
        // exit the loop and avoid starting further actions.
        if (mPriv->currentPhase != phase) {
            ret = false;
            break;
        }

        std::vector<std::pair<Device, Action>> actions = mPriv->getActions(phase, level);

        for (auto& ait : actions) {
            // for each action in that level
            Device& device = ait.first;
            Action& action = ait.second;

            // If current phase was changed by some other thread, we should
            // exit the loop and avoid starting further actions.
            if (mPriv->currentPhase != phase) {
                ret = false;
                break;
            }

            yCInfo(YRI_ROBOT) << "Executing" << ActionTypeToString(action.type()) << "action, level" << action.level() << "on device" << device.name() << "with parameters" << action.params();

            switch (action.type()) {
            case ActionTypeConfigure:
                if (!mPriv->configure(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run configure action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCalibrate:
                if (!mPriv->calibrate(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run calibrate action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeAttach:
                if (!mPriv->attach(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run attach action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeAbort:
                if (!mPriv->abort(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run abort action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeDetach:
                if (!mPriv->detach(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run detach action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypePark:
                if (!mPriv->park(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run park action on device" << device.name();
                    ret = false;
                }
                break;
            case ActionTypeCustom:
                if (!mPriv->custom(device, action.params())) {
                    yCError(YRI_ROBOT) << "Cannot run custom action on device" << device.name();
                    ret = false;
                }
                break;
            default:
                yCWarning(YRI_ROBOT) << "Unhandled action" << ActionTypeToString(action.type());
                ret = false;
                break;
            }
        }

        yCInfo(YRI_ROBOT) << "All actions for action level" << level << "of" << ActionPhaseToString(phase) << "phase started. Waiting for unfinished actions.";

        // Join parallel threads
        for (auto& device : devices()) {
            device.joinThreads();
            // yCDebug() << "All actions for device" << device.name() << "at level()" << level << "finished";
        }

        yCInfo(YRI_ROBOT) << "All actions for action level" << level << "of" << ActionPhaseToString(phase) << "phase finished.";
    }

    if (!ret) {
        yCWarning(YRI_ROBOT) << "There was some problem running actions for" << ActionPhaseToString(phase) << "phase . Please check the log and your configuration";
    }

    if (phase == ActionPhaseShutdown) {
        if (!mPriv->closeDevices()) {
            yCError(YRI_ROBOT) << "One or more devices failed closing";
            return false;
        }
    }

    yCInfo(YRI_ROBOT) << ActionPhaseToString(phase) << "phase finished.";

    return ret;
}

yarp::robotinterface::ActionPhase yarp::robotinterface::Robot::currentPhase() const
{
    return mPriv->currentPhase;
}

int yarp::robotinterface::Robot::currentLevel() const
{
    return mPriv->currentLevel;
}

bool yarp::robotinterface::Robot::hasParam(const std::string& name) const
{
    return yarp::robotinterface::hasParam(mPriv->params, name);
}

std::string yarp::robotinterface::Robot::findParam(const std::string& name) const
{
    return yarp::robotinterface::findParam(mPriv->params, name);
}
