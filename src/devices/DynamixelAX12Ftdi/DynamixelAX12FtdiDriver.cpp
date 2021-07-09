/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2010 Ze Ji
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "DynamixelAX12FtdiDriver.h"

#include <yarp/os/Value.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>

using namespace yarp::os;
using namespace yarp::dev;

#define BOOL_EXIT_FAILURE false

YARP_LOG_COMPONENT(DYNAMIXELAX12FTDIDRIVER, "yarp.devices.DynamixelAX12FtdiDriver")


bool NOT_YET_IMPLEMENTED(const char *txt) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "%s not yet implemented for DynamixelAX12FtdiDriver", txt);
    return false;
}

DynamixelAX12FtdiDriver::DynamixelAX12FtdiDriver() :
        deviceOpen(false),
        jointNumbers((unsigned char *) malloc(16 * sizeof (unsigned char))),
        numOfAxes(16),
        positions((double *) malloc(numOfAxes * sizeof (double))),
        speeds((double *) malloc(numOfAxes * sizeof (double))),
        devlist(NULL),
        torques((int *) malloc(numOfAxes * sizeof (int)))
{
    // initialise
    jointNumbers[0] = 0x65;
    jointNumbers[1] = 0x74;
    jointNumbers[2] = 0x84;
    jointNumbers[3] = 0x66;
    jointNumbers[4] = 0x75;
    jointNumbers[5] = 0x85;
    jointNumbers[6] = 0x67;
    jointNumbers[7] = 0x76;
    jointNumbers[8] = 0x86;
    jointNumbers[9] = 0x6B;
    jointNumbers[10] = 0x77;
    jointNumbers[11] = 0x87;
    jointNumbers[12] = 0x6A;
    jointNumbers[13] = 0x68;
    jointNumbers[14] = 0x69;
    jointNumbers[15] = 0x6C;

    ftdi_init(&ftdic);
}

DynamixelAX12FtdiDriver::~DynamixelAX12FtdiDriver() {
    ftdi_list_free(&devlist);
    ftdi_deinit(&ftdic);
    free(jointNumbers);
    free(positions);
    free(speeds);
    free(torques);
}

bool DynamixelAX12FtdiDriver::open(yarp::os::Searchable& config) {

    mutex.lock();

    FtdiDeviceSettings ftdiSetting;
    strcpy(ftdiSetting.description, config.check("FTDI_DESCRIPTION", Value("FT232R USB UART"), "Ftdi device description").asString().c_str());
    strcpy(ftdiSetting.serial, config.check("FTDI_SERIAL", Value("A7003MhG"), "Ftdi device serial").asString().c_str());
    strcpy(ftdiSetting.manufacturer, config.check("FTDI_MANUFACTURER", Value("FTDI"), "Ftdi device manufacturer").asString().c_str());
    ftdiSetting.baudrate = config.check("baudrate", Value(57600), "Specifies the baudrate at which the communication port operates.").asInt32();
    ftdiSetting.vendor = config.check("ftdivendor", Value(0x0403), "USB device vendor. 0x0403 normally. Can be found by lsusb on linux").asInt32();
    ftdiSetting.product = config.check("ftdiproduct", Value(0x6001), "USB device product number. 0x6001 normally. Can be found by lsusb on linux").asInt32();
    ftdiSetting.flowctrol = config.check("flowctrl", Value(SIO_DISABLE_FLOW_CTRL), "flow control to use. Should be SIO_DISABLE_FLOW_CTRL = 0x0, SIO_RTS_CTS_HS = 0x1 << 8, SIO_DTR_DSR_HS = 0x2 << 8, or SIO_XON_XOFF_HS = 0x4 << 8").asInt32();
    ftdiSetting.write_chunksize = 3;
    ftdiSetting.read_chunksize = 256;

    yCTrace(DYNAMIXELAX12FTDIDRIVER, "DynamixelAx12Driver::initialize");
    yCInfo(DYNAMIXELAX12FTDIDRIVER, "Opening DynamixelAx12 Device");

    int retCode;

    char manufacturer[128], description[128];
    char serial[128];
    //int n = 0; // number of devices found

    retCode = ftdi_usb_find_all(&ftdic, &devlist, ftdiSetting.vendor, ftdiSetting.product);
    switch (retCode) {
        case -1:
            yCError(DYNAMIXELAX12FTDIDRIVER, "usb_find_busses() failed");
            return BOOL_EXIT_FAILURE;
        case -2:
            yCError(DYNAMIXELAX12FTDIDRIVER, "usb_find_devices() failed");
            return BOOL_EXIT_FAILURE;
        case -3:
            yCError(DYNAMIXELAX12FTDIDRIVER, "out of memory");
            return BOOL_EXIT_FAILURE;
        case 0:
            yCError(DYNAMIXELAX12FTDIDRIVER, "No device is found. Check connection.");
            return BOOL_EXIT_FAILURE;
        default:
            yCInfo(DYNAMIXELAX12FTDIDRIVER, "%d devices have been found.", retCode);
            break;
    }

    int ret = 0;
    int i = 0;

    for (curdev = devlist; curdev != NULL; i++) {
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Checking device: %d", i);

        if ((retCode = ftdi_usb_get_strings(&ftdic, curdev->dev, manufacturer, 128, description, 128, serial, 128)) < 0) {
            yCError(DYNAMIXELAX12FTDIDRIVER, "ftdi_usb_get_strings failed: %d (%s)", ret, ftdi_get_error_string(&ftdic));
        }
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Manufacturer: %s, Description: %s, Serial %s", manufacturer, description, serial);

        // check if the current device is the right one
        if (strcmp(serial, ftdiSetting.serial) == 0) {
            break;
        }
        curdev = curdev->next;
    }

    if (curdev == NULL) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "No Dynamixel device is found. Check connection.");
        return BOOL_EXIT_FAILURE;
    }

    // open and reset everything in case we messed everything up with a control-c..
    if ((retCode = ftdi_usb_open_dev(&ftdic, curdev->dev)) < 0) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "unable to open ftdi device: %d (%s)", retCode, ftdi_get_error_string(&ftdic));
        return BOOL_EXIT_FAILURE;
    }
    deviceOpen = true; // Only set to be able to do close()

    this->close();

    // Now open again
    if ((retCode = ftdi_usb_open_dev(&ftdic, curdev->dev)) < 0) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "unable to open ftdi device: %d (%s)", retCode, ftdi_get_error_string(&ftdic));
        return BOOL_EXIT_FAILURE;
    }

    // To test connection, read out FTDIChip-ID of R type chips
    if (ftdic.type == TYPE_R) {
        unsigned int chipid;
        if (ftdi_read_chipid(&ftdic, &chipid) == 0) {
            yCInfo(DYNAMIXELAX12FTDIDRIVER, "FTDI Device open. FTDI chipid: %X", chipid);
        } else {
            yCError(DYNAMIXELAX12FTDIDRIVER, "Unable to access FTDI device!");
            return BOOL_EXIT_FAILURE;
        }
    }
    deviceOpen = true;

    // We have an open device and it is accessible, so set parameters

    if ((retCode = ftdi_set_baudrate(&ftdic, ftdiSetting.baudrate)) != 0)
        yCError(DYNAMIXELAX12FTDIDRIVER, "Error setting baudrate, ret=%d", retCode);
    if (ftdi_set_line_property(&ftdic, BITS_8, STOP_BIT_1, NONE) == -1)
        yCError(DYNAMIXELAX12FTDIDRIVER, "Error setting connection properties");
    if (ftdi_setflowctrl(&ftdic, SIO_DISABLE_FLOW_CTRL) == -1)
        yCError(DYNAMIXELAX12FTDIDRIVER, "Error setting flow control");

    // Set chunk sizes for in and out
    ftdi_write_data_set_chunksize(&ftdic, ftdiSetting.write_chunksize);
    ftdi_read_data_set_chunksize(&ftdic, ftdiSetting.read_chunksize);

    // Everything ready to rumble

    mutex.unlock();

    return true;
}

bool DynamixelAX12FtdiDriver::close() {

    // First check if we have an open device
    if (deviceOpen) {
        // Yes, we have, so purge the buffers, reset the device and then close it
        ftdi_usb_purge_buffers(&ftdic);
        //ftdi_usb_reset(ftdic.usb_dev);
        ftdi_usb_reset(&ftdic);
        ftdi_usb_close(&ftdic);
    } else {
        // We don't seem to have a device open at this time
        yCError(DYNAMIXELAX12FTDIDRIVER, "close():No device open to be be closed!");
        return false;
    }
    return true;
}

/**
 *
 */
bool DynamixelAX12FtdiDriver::configure(yarp::os::Searchable& config) {
    yarp::os::Value indexValue = config.find("SENSORINDEX");
    yarp::os::Bottle *indexBottle = indexValue.asList();
    this->initMotorIndex(indexBottle);
    return true;
}

int DynamixelAX12FtdiDriver::syncSendCommand(unsigned char id, unsigned char inst[], int size, unsigned char ret[], int &retSize) {
    int r = 0;
    mutex.lock();
    if (!ftdi_usb_purge_buffers(&ftdic)) {
        r = sendCommand(id, inst, size, ret, retSize);
    }

    mutex.unlock();
    return r;
}

int DynamixelAX12FtdiDriver::sendCommand(unsigned char id, unsigned char inst[], int size, unsigned char ret[], int &retSize) {

    int i;
    int retCode = 0;
    bool badAnswer = false;
    unsigned char command[16];
    unsigned char chksum = 0x00;
    unsigned char header[4];
    unsigned char body[256];

    // Build packet header

    command[0] = 0xFF; // Header bytes
    command[1] = 0xFF;
    command[2] = id; // Id of device
    command[3] = size + 1; // Size of instruction plus checksum byte

    // Add instruction
    for (i = 0; i < size; i++) {
        command[4 + i] = inst[i];
        chksum += inst[i];
    }

    // Calculate checksum
    command[size + 5 - 1] = ~(chksum + command[2] + command[3]);
    yCDebug(DYNAMIXELAX12FTDIDRIVER, "Packet sent: ");
    for (i = 0; i < size + 5; i++) {
        yCDebug(DYNAMIXELAX12FTDIDRIVER, "    %X", command[i]);
    }
    // Write data to device
    retCode = ftdi_write_data(&ftdic, command, size + 5);

    // Now receive return packet
    if (id != AX12_BROADCAST_ID) {
        do {
            //retCode stores the number of bytes read, which may not correspond to the number of bytes
            //one wants to read (the third parameter). This is a timing issue, so to learn how to fix
            //it, I should send a command and then continually prints the bytes that I get back. At
            //some point, I will get back the correct number of bytes. After doing this enough times,
            //I should work out the timing issue
            retCode = ftdi_read_data(&ftdic, header, 4);
        }            // retCode: <0,:     error code from usb_bulk_read()
            //          0:     no data was available
            //          >0:     number of bytes read
            /// TODO check if it works with Kaspar. Originally made by Sven. Need testing
        while (retCode == 0); // not needed. as it will hang if no device is connected

        if (retCode < 0) {
            yCError(DYNAMIXELAX12FTDIDRIVER, "Error while reading header of status packet! From usb_bulk_read() (%d)", retCode);
            return 0;
        } else if (retCode < 4) {
            yCError(DYNAMIXELAX12FTDIDRIVER, "Error while reading header of status packet! (%d)", retCode);
            return 0;
        } else {
            retCode = 0;
            if (header[0] != 0xFF || header[1] != 0xFF) {
                badAnswer = true;
                yCInfo(DYNAMIXELAX12FTDIDRIVER, "Received data without header bytes: ");
            } else if (header[2] != id) {
                badAnswer = true;
                yCInfo(DYNAMIXELAX12FTDIDRIVER, "Received status from wrong device (expected %d): ", id);
            } else {
                // Now that we know a well formed packet is arriving, read remaining bytes
                do {
                    retCode = ftdi_read_data(&ftdic, body, header[3]);
                } while (retCode == 0);

                if (retCode != header[3]) {
                    badAnswer = true;
                    yCInfo(DYNAMIXELAX12FTDIDRIVER, "Received data with wrong length: ");
                } else {
                    //check checksum
                    chksum = 0;
                    for (i = 0; i < retCode - 1; i++) chksum += body[i];
                    if (body[retCode - 1] != (unsigned char) ~(chksum + header[2] + header[3])) {
                        badAnswer = true;
                        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Received data with wrong checksum (%X != %X): ", body[retCode - 1], (unsigned char) ~(chksum + header[2] + header[3]));
                    } else {
                        // Packet ok, so return the instruction part without checksum
                        for (i = 0; i < retCode - 1; i++) {
                            ret[i] = body[i];
                        }
                        // retSize = retCode;        // Sven's original code.
                        retSize = retCode - 1; // retSize should be retCode - 1, as checksum is not included
                        return 1;
                    }
                }
            }
        }

        // Print packet if something was wrong
        if (badAnswer) {
            for (i = 0; i < 4; i++) {
                yCInfo(DYNAMIXELAX12FTDIDRIVER, "%X ", header[i]);
            }
            for (i = 0; i < retCode; i++) {
                yCInfo(DYNAMIXELAX12FTDIDRIVER, "%X ", body[i]);
            }
            return 0;
        }
    }

    return 1;
}

ErrorCode DynamixelAX12FtdiDriver::checkAnswerPacket(unsigned char* packet, const char*& message) {
    if (packet[0] & 1) {
        message = "Voltage out of operating voltage range!";
        return VOLTAGE_ERROR;
    }
    if (packet[0] & 2) {
        message = "Goal position outside angle limits!";
        return ANGLE_ERROR;
    }
    if (packet[0] & 4) {
        message = "Current temperature outside operating temperature!";
        return OVERHEATING_ERROR;
    }
    if (packet[0] & 8) {
        message = "Instruction out of defined range!";
        return RANGE_ERROR;
    }
    if (packet[0] & 16) {
        message = "Checksum of instruction package incorrect!";
        return CHECKSUM_ERROR;
    }
    if (packet[0] & 32) {
        message = "Specified torque can't control the applied load!";
        return OVERLOAD_ERROR;
    }
    if (packet[0] & 64) {
        message = "Undefined instruction or missing Reg_Write instruction!";
        return INSTRUCTION_ERROR;
    }
    message = "";
    return OK;
}

/**
 */
int DynamixelAX12FtdiDriver::readParameter(unsigned char id, unsigned char param) {
    // Read instruction has 2 parameters: The starting address of the parameter and the number of bytes to read
    // Both are encoded in param (> 100 = 2 bytes)
    unsigned char packet[] = {INST_READ, 0, 1};

    unsigned char answerPacket[256];
    int answerSize;
    const char* errorMessage;
    ErrorCode errCode;

    if (param > 99) {
        // We have to read 2 bytes
        packet[1] = param - 100;
        packet[2] = 2;
    } else {
        // We have to read 1 bytes
        packet[1] = param;
    }

    // Send request
    //sendCommand(id, packet, 3, answerPacket, answerSize);
    syncSendCommand(id, packet, 3, answerPacket, answerSize);
    // Check for submitted error code
    if ((errCode = checkAnswerPacket(answerPacket, errorMessage)) != OK) {
        // We received an error code
        return -1;
    } else {
        // No error
        return 1;
    }
}

bool DynamixelAX12FtdiDriver::getAxes(int *ax) {
    *ax = numOfAxes;
    return true;
}

bool DynamixelAX12FtdiDriver::positionMove(int j, double ref) {
    double speed;
    int blankReturnSize = -1;
    unsigned char blankReturn[] = {0, 0, 0};
    getRefSpeed(j, &speed);
    const int instl = 5 + 1;

    unsigned char inst[instl] = {INST_WRITE, CT_GOAL_POSITION, (unsigned char) (normalisePosition(ref)&0xFF), (unsigned char) ((normalisePosition(ref) >> 8) & 0xFF), (unsigned char) (normaliseSpeed(speed)&0xFF), (unsigned char) ((normaliseSpeed(speed) >> 8)&0xFF)};

    positions[j] = ref;

    return syncSendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
}

/**
 * @param refs should be in range [1 300]
 */
bool DynamixelAX12FtdiDriver::positionMove(const double *refs) {
    bool t = true;
    for (int k = 0; k < numOfAxes; k++) {
        if (!this->positionMove(k, refs[k]))
            t = false;
    }
    return t;
}

bool DynamixelAX12FtdiDriver::relativeMove(int j, double delta) {
    double v = positions[j];
    if (getEncoder(j, &v)) {
        return this->positionMove(j, v + delta);
    } else
        return false;
}

bool DynamixelAX12FtdiDriver::relativeMove(const double *deltas) {
    bool t = true;
    for (int k = 0; k < numOfAxes; k++) {
        if (!this->positionMove(k, positions[k] + deltas[k]))
            t = false;
    }
    return t;
}

bool DynamixelAX12FtdiDriver::checkMotionDone(int j, bool *flag) {
    double v = 0;
    bool t = true;

    t = getEncoder(j, &v);
    *flag = (std::fabs(v - positions[j]) < MOTION_COMPLETION_TOLERANCE);
    return t;
}

bool DynamixelAX12FtdiDriver::checkMotionDone(bool *flag) {
    bool t = true;
    bool tmp_done(false), all_done(true);
    for (int k = 0; k < numOfAxes; k++)
    {
        if (!this->checkMotionDone(k, &tmp_done))
            t = false;
        all_done &= tmp_done;
    }
    *flag = all_done;
    return t;
}

bool DynamixelAX12FtdiDriver::setRefSpeed(int j, double sp) {
    if (sp < 1) {
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid speed value, should be from 1 to 114");
        speeds[j] = 1;
        return false;
    } else if (sp > 114) {
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid speed value, should be from 1 to 114");
        speeds[j] = 114;
        return false;
    } else {
        speeds[j] = sp;
        return true;
    }
}

bool DynamixelAX12FtdiDriver::setRefSpeeds(const double *spds) {
    bool t = true;
    for (int k = 0; k < numOfAxes; k++) {
        if (!setRefSpeed(k, spds[k]))
            t = false;
    }
    return t;
}

/**
 * not implemented
 */
bool DynamixelAX12FtdiDriver::setRefAcceleration(int j, double acc) {
    return NOT_YET_IMPLEMENTED("setRefAcceleration");
}

/**
 * not implemented
 */
bool DynamixelAX12FtdiDriver::setRefAccelerations(const double *accs) {
    return NOT_YET_IMPLEMENTED("setRefAccelerations");
}

bool DynamixelAX12FtdiDriver::getRefSpeed(int j, double *ref) {
    *ref = speeds[j];
    return true;
}

bool DynamixelAX12FtdiDriver::getRefSpeeds(double *spds) {
    for (int k = 0; k < numOfAxes; k++) {
        spds[k] = speeds[k];
    }
    return true;
}

/**
 * not implemented
 */
bool DynamixelAX12FtdiDriver::getRefAcceleration(int j, double *acc) {
    return NOT_YET_IMPLEMENTED("getRefAcceleration");
}

/**
 * not implemented
 */
bool DynamixelAX12FtdiDriver::getRefAccelerations(double *accs) {
    return NOT_YET_IMPLEMENTED("getRefAccelerations");
}

bool DynamixelAX12FtdiDriver::stop(int j) {
    return NOT_YET_IMPLEMENTED("stop");
}

bool DynamixelAX12FtdiDriver::stop() {
    for (int i = 0; i < numOfAxes; i++) {
        if (!stop(i))
            return false;
    }
    return true;
}

int DynamixelAX12FtdiDriver::normalisePosition(double position) {
    if (position < 0) {
        position = 0;
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid position value, should be from 0 to 300");
    } else if (position > 300) {
        position = 300;
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid position value, should be from 0 to 300");
    }

    return (int) (1024 * position / 301);
}

int DynamixelAX12FtdiDriver::normaliseSpeed(double speed) {
    // speed -> [1 114] RPM
    // when speed is 0, it is maximum possible speed, no speed control. so it is not useful here
    if (speed < 1) {
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid speed value, should be from 1 to 114");
        speed = 1;
    } else if (speed > 114) {
        yCInfo(DYNAMIXELAX12FTDIDRIVER, "Invalid speed value, should be from 1 to 114");
        speed = 114;
    }
    return (int) (1024 * speed / 114 - 1);
}

bool DynamixelAX12FtdiDriver::getRefTorques(double *t) {
    for (int k = 0; k < numOfAxes; k++) {
        t[k] = torques[k];
    }
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is just the MAX tourque set for the ax12 servo.");
    return true;
}

bool DynamixelAX12FtdiDriver::getRefTorque(int j, double *t) {
    *t = torques[j];
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is just the MAX tourque set for the ax12 servo.");
    return true;
}

bool DynamixelAX12FtdiDriver::setTorques(const double *t) {
    bool tt = true;
    for (int k = 0; k < numOfAxes; k++) {
        if (!this->setTorque(k, t[k]))
            tt = false;
    }
    return tt;
}

/**
 * @param t has not real unit. Although it can be roughly estimated based on the maximum toruqe as specified in the document
 * Here t should be only set from 0 - 0x3FF (1023) as int
 */
bool DynamixelAX12FtdiDriver::setTorque(int j, double t) {
    if (t < 0) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "torque (%d) should in range [0 1023] or [0 0x3FF]. t is set to 0 here", (int)t);
        t = 0;
    } else if (t > 1023) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "torque (%d) should in range [0 1023] or [0 0x3FF]. t is set to 1023 here", (int)t);
        t = 1023;
    }

    int blankReturnSize = -1;
    unsigned char blankReturn[] = {0, 0, 0};
    const int instl = 3 + 1;

    unsigned char inst[instl] = {INST_WRITE, CT_MAX_TORQUE, (unsigned char) ((int) t & 0xFF), (unsigned char) (((int) t << 8) & 0x03)};
    //return sendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
    return syncSendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
}

bool DynamixelAX12FtdiDriver::setTorquePid(int j, const Pid &pid) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setTorquePid");
}

bool DynamixelAX12FtdiDriver::getTorque(int j, double *t) {
    bool ret;

    int blankReturnSize = -1;
    unsigned char blankReturn[] = {0, 0, 0};

    const int instl = 3;
    unsigned char inst[instl] = {INST_READ, (unsigned char) CT_PRESENT_LOAD, (unsigned char) 2};
    //ret = sendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
    ret = syncSendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);

    const char* message = "";
    if (checkAnswerPacket(blankReturn, message) == OK) {
        //int load = (blankReturn[2]&0b00000011)*256 + blankReturn[1];
        int load = (blankReturn[2]&0X03)*256 + blankReturn[1];
        //load *= (((blankReturn[2] >> 2)&0b000001) ? -1 : 1);
        load *= (((blankReturn[2] >> 2)&0X01) ? -1 : 1);
        *t = load;
    } else {
        yCError(DYNAMIXELAX12FTDIDRIVER, "%s", message);
        return false;
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::getTorques(double *t) {
    int k = 0;
    bool tt = true;
    for (k = 0; k < numOfAxes; k++) {
        if (!getTorque(k, &t[k]))
            tt = false;
    }
    return tt;
}

bool DynamixelAX12FtdiDriver::setTorquePids(const Pid *pids) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setTorquePids");
}

bool DynamixelAX12FtdiDriver::setTorqueErrorLimit(int j, double limit) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setTorqueErrorLimit");
}

bool DynamixelAX12FtdiDriver::setTorqueErrorLimits(const double *limits) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setTorqueErrorLimits");
}

bool DynamixelAX12FtdiDriver::getTorqueError(int j, double *err) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorqueError");
}

bool DynamixelAX12FtdiDriver::getTorqueErrors(double *errs) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorqueErrors");
}

bool DynamixelAX12FtdiDriver::getTorquePidOutput(int j, double *out) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorquePidOutput");
}

bool DynamixelAX12FtdiDriver::getTorquePidOutputs(double *outs) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorquePidOutputs");
}

bool DynamixelAX12FtdiDriver::getTorquePid(int j, Pid *pid) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorquePid");
}

bool DynamixelAX12FtdiDriver::getTorquePids(Pid *pids) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorquePids");
}

bool DynamixelAX12FtdiDriver::getTorqueErrorLimit(int j, double *limit) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorqueErrorLimit");
}

bool DynamixelAX12FtdiDriver::getTorqueErrorLimits(double *limits) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getTorqueErrorLimits");
}

bool DynamixelAX12FtdiDriver::resetTorquePid(int j) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("resetTorquePid");
}

bool DynamixelAX12FtdiDriver::disableTorquePid(int j) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("disableTorquePid");
}

bool DynamixelAX12FtdiDriver::enableTorquePid(int j) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("enableTorquePid");
}

bool DynamixelAX12FtdiDriver::setTorqueOffset(int j, double v) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setTorqueOffset");
}

bool DynamixelAX12FtdiDriver::getBemfParam(int j, double *bemf) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("getBemfParam");
}

bool DynamixelAX12FtdiDriver::setBemfParam(int j, double bemf) {
    yCError(DYNAMIXELAX12FTDIDRIVER, "Note: AX12 does not support torque control mode. This is only used to get torque feedback.");
    return NOT_YET_IMPLEMENTED("setBemfParam");
}

bool DynamixelAX12FtdiDriver::resetEncoder(int j) {
    return NOT_YET_IMPLEMENTED("resetEncoder");
}

bool DynamixelAX12FtdiDriver::resetEncoders() {
    return NOT_YET_IMPLEMENTED("resetEncoders");
}

bool DynamixelAX12FtdiDriver::setEncoder(int j, double val) {
    return NOT_YET_IMPLEMENTED("setEncoder");
}

bool DynamixelAX12FtdiDriver::setEncoders(const double *vals) {
    return NOT_YET_IMPLEMENTED("setEncoders");
}

bool DynamixelAX12FtdiDriver::setRefTorques(const double* t) {
    return NOT_YET_IMPLEMENTED("setRefTorques");
}

bool DynamixelAX12FtdiDriver::setRefTorque(int j, double t) {
    return NOT_YET_IMPLEMENTED("setRefTorque");
}

bool DynamixelAX12FtdiDriver::getTorqueRange(int j, double* min, double* max) {
    return NOT_YET_IMPLEMENTED("getTorqueRange");
}

bool DynamixelAX12FtdiDriver::getTorqueRanges(double* min, double* max) {
    return NOT_YET_IMPLEMENTED("getTorqueRanges");
}

bool DynamixelAX12FtdiDriver::getEncoder(int j, double *v) {
    *v = -1; // invalid value for encoder of AX12 motor
    int ret = 1;

    int blankReturnSize = -1;
    unsigned char blankReturn[] = {0, 0, 0};

    const int instl = 3;
    unsigned char inst[instl] = {INST_READ, (unsigned char) CT_PRESENT_POSITION, (unsigned char) 2};
    //ret = sendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
    ret = syncSendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);

    int pos = 0;
    if (ret == 1) {
        const char* message = "";
        if (checkAnswerPacket(blankReturn, message) == OK) {
            pos = ((int) blankReturn[2])*256 + blankReturn[1];
            *v = ((double) pos)*300.0 / 1024.0;
            return true;
        } else {
            yCError(DYNAMIXELAX12FTDIDRIVER, "%s", message);
            return false;
        }

    }        // pos = (blankReturn[1]&0b00000011)*0x100 + blankReturn[0];
    else
        return false;
}

bool DynamixelAX12FtdiDriver::getEncoders(double *encs) {
    int k = 0;
    bool tt = true;

    for (k = 0; k < numOfAxes; k++) {
        if (!getEncoder(k, &encs[k])) {
            tt = false;
        }
    }
    return tt;
}

bool DynamixelAX12FtdiDriver::getEncoderSpeed(int j, double *sp) {
    bool ret;

    int blankReturnSize = -1;
    unsigned char blankReturn[] = {0, 0, 0};
    const int instl = 3;
    unsigned char inst[instl] = {INST_READ, (unsigned char) CT_PRESENT_SPEED, (unsigned char) 2};
    //ret = sendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);
    ret = syncSendCommand(jointNumbers[j], inst, instl, blankReturn, blankReturnSize);

    double speed = 0;
    const char* message = "";
    if (checkAnswerPacket(blankReturn, message) == OK) {
        //speed = (blankReturn[2]&0b00000011)*0xFF + blankReturn[1];
        speed = (blankReturn[2]&0X03)*0xFF + blankReturn[1];

        //speed *= (((blankReturn[1] >> 2)&0b000001) ? -1 : 1);
        speed *= (((blankReturn[1] >> 2)&0X01) ? -1 : 1);

        *sp = speed * 113 / 1024 + 1; /// TODO should be changed. not very accurate, though close
    } else {
        yCError(DYNAMIXELAX12FTDIDRIVER, "%s", message);
        return false;
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::getEncoderSpeeds(double *spds) {
    int k = 0;
    bool tt = true;
    for (k = 0; k < numOfAxes; k++) {
        if (!getEncoderSpeed(k, &spds[k]))
            tt = false;
    }
    return tt;
}

bool DynamixelAX12FtdiDriver::getEncoderAcceleration(int j, double *spds) {
    return NOT_YET_IMPLEMENTED("getEncoderAcceleration");
}

bool DynamixelAX12FtdiDriver::getEncoderAccelerations(double *accs) {
    return NOT_YET_IMPLEMENTED("getEncoderAccelerations");
}

bool DynamixelAX12FtdiDriver::initMotorIndex(yarp::os::Bottle *sensorIndex) {
    int s = sensorIndex->size() - 1;
    if (s != sensorIndex->get(0).asInt32()) {
        yCError(DYNAMIXELAX12FTDIDRIVER, "sensor number does not match the real number in configuration file");
        return false;
    }
    numOfAxes = s;
    unsigned char *tmp = NULL;
    tmp = (unsigned char *) realloc(jointNumbers, numOfAxes * sizeof (unsigned char));
    if (tmp != NULL) {
        //    jointNumbers = {0x65, 0x74, 0x84, 0x66, 0x75, 0x85, 0x67, 0x76, 0x86, 0x6B, 0x77, 0x87, 0x6A, 0x68, 0x69, 0x6C};
        jointNumbers = tmp;
    } else {
        return false;
    }
    double *tmpp = NULL;
    tmpp = (double *) realloc(positions, numOfAxes * sizeof (double));
    if (tmpp != NULL) {
        positions = tmpp;
    } else {
        return false;
    }

    double *tmps = NULL;
    tmps = (double *) realloc(speeds, numOfAxes * sizeof (double));
    if (tmps != NULL) {
        speeds = tmps;
    } else {
        return false;
    }

    int *tmpt = NULL;
    tmpt = (int *) realloc(torques, numOfAxes * sizeof (int));
    if (tmpt != NULL) {
        torques = tmpt;
    } else {
        return false;
    }

    for (int i = 0; i < numOfAxes; i++) {
        jointNumbers[i] = (unsigned char) (sensorIndex->get(i + 1).asInt32());
    }
    return true;
}

bool DynamixelAX12FtdiDriver::positionMove(const int n_joint, const int *joints, const double *refs)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= positionMove(joints[j], refs[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::relativeMove(const int n_joint, const int *joints, const double *deltas)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= relativeMove(joints[j], deltas[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::checkMotionDone(const int n_joint, const int *joints, bool *flag)
{
    bool ret = true;
    bool tmp_joint(false), tmp_device(true);
    for(int j=0; j<n_joint; j++)
    {
        ret &= checkMotionDone(joints[j], &tmp_joint);
        tmp_device &= tmp_joint;
    }
    *flag = tmp_device;
    return ret;
}

bool DynamixelAX12FtdiDriver::setRefSpeeds(const int n_joint, const int *joints, const double *spds)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= setRefSpeed(joints[j], spds[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= setRefAcceleration(joints[j], accs[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::getRefSpeeds(const int n_joint, const int *joints, double *spds)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= getRefSpeed(joints[j], &spds[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= getRefSpeed(joints[j], &accs[j]);
    }
    return ret;
}

bool DynamixelAX12FtdiDriver::stop(const int n_joint, const int *joints)
{
    bool ret = true;
    for(int j=0; j<n_joint; j++)
    {
        ret &= stop(joints[j]);
    }
    return ret;
}
