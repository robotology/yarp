/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_CARRIER_SM2VEC_CONVERTER_H
#define YARP_CARRIER_SM2VEC_CONVERTER_H

#include <yarp/os/Bottle.h>
#include <yarp/os/Things.h>
#include <yarp/os/MonitorObject.h>
#include <yarp/sig/Vector.h>

 /**
  * @ingroup portmonitors_lists
  * \brief `sensorMeasurements_to_vector`:  Documentation to be added
  * example usage:
  * yarp connect /tracking/measures:o /in tcp+recv.portmonitor+type.dll+file.sensorMeasurements_to_vector
  */
class SensorMeasurements_to_vector : public yarp::os::MonitorObject
{
    yarp::sig::Vector    out;
    yarp::os::Things     th;

public:
    bool create(const yarp::os::Property& options) override;
    void destroy() override;

    bool setparam(const yarp::os::Property& params) override;
    bool getparam(yarp::os::Property& params) override;

    bool accept(yarp::os::Things& thing) override;
    yarp::os::Things& update(yarp::os::Things& thing) override;
};

#endif  // YARP_CARRIER_SM2VEC_CONVERTER_H
