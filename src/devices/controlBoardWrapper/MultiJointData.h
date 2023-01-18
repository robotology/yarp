/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDWRAPPER_MULTIJOINTDATA_H
#define YARP_DEV_CONTROLBOARDWRAPPER_MULTIJOINTDATA_H

#include "SubDevice.h"

class MultiJointData
{
public:
    size_t deviceNum {0};
    size_t maxJointsNumForDevice {0};

    int* subdev_jointsVectorLen {nullptr}; // number of joints belonging to each subdevice
    int** jointNumbers {nullptr};
    int** modes {nullptr};
    double** values {nullptr};
    SubDevice** subdevices_p {nullptr};

    MultiJointData() = default;

    void resize(int _deviceNum, int _maxJointsNumForDevice, WrappedDevice* _device)
    {
        deviceNum = _deviceNum;
        maxJointsNumForDevice = _maxJointsNumForDevice;
        subdev_jointsVectorLen = new int[deviceNum];
        jointNumbers = new int*[deviceNum];                            // alloc a vector of pointers
        jointNumbers[0] = new int[deviceNum * _maxJointsNumForDevice]; // alloc real memory for data

        modes = new int*[deviceNum];                            // alloc a vector of pointers
        modes[0] = new int[deviceNum * _maxJointsNumForDevice]; // alloc real memory for data

        values = new double*[deviceNum];                            // alloc a vector of pointers
        values[0] = new double[deviceNum * _maxJointsNumForDevice]; // alloc real memory for data

        subdevices_p = new SubDevice*[deviceNum];
        subdevices_p[0] = _device->getSubdevice(0);

        for (size_t i = 1; i < deviceNum; i++) {
            jointNumbers[i] = jointNumbers[i - 1] + _maxJointsNumForDevice; // set pointer to correct location
            values[i] = values[i - 1] + _maxJointsNumForDevice;             // set pointer to correct location
            modes[i] = modes[i - 1] + _maxJointsNumForDevice;               // set pointer to correct location
            subdevices_p[i] = _device->getSubdevice(i);
        }
    }

    void destroy()
    {
        // release matrix memory
        delete[] jointNumbers[0];
        delete[] values[0];
        delete[] modes[0];

        // release vector of pointers
        delete[] jointNumbers;
        delete[] values;
        delete[] modes;

        // delete other vectors
        delete[] subdev_jointsVectorLen;
        delete[] subdevices_p;
    }
};

#endif // YARP_DEV_CONTROLBOARDWRAPPER_MULTIJOINTDATA_H
