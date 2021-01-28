// Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
// All rights reserved.
//
// This software may be modified and distributed under the terms of the
// BSD-3-Clause license. See the accompanying LICENSE file for details.

%define RESET_CONSTANTS_IN_EXTENDED_ANALOG_SENSOR_INTERFACE
#undef ThreeAxisGyroscopes_EXTENDED_INTERFACE
#undef ThreeAxisLinearAccelerometers_EXTENDED_INTERFACE
#undef ThreeAxisMagnetometers_EXTENDED_INTERFACE
#undef OrientationSensors_EXTENDED_INTERFACE
#undef TemperatureSensors_EXTENDED_INTERFACE
#undef SixAxisForceTorqueSensors_EXTENDED_INTERFACE
#undef ContactLoadCellArrays_EXTENDED_INTERFACE
#undef EncoderArrays_EXTENDED_INTERFACE
#undef SkinPatches_EXTENDED_INTERFACE
%enddef

%define EXTENDED_ANALOG_SENSOR_INTERFACE(sensor)

RESET_CONSTANTS_IN_EXTENDED_ANALOG_SENSOR_INTERFACE

#define sensor ## _EXTENDED_INTERFACE 1

    std::string get ## sensor ## Name(int sens_index) const {
        std::string name;
        bool ok = self->get ## sensor ## Name(sens_index,name);
        if (!ok) return "unknown";
        return name;
    }

#if !ContactLoadCellArray_EXTENDED_INTERFACE \
 && !EncoderArray_EXTENDED_INTERFACE \
 && !SkinPatch_EXTENDED_INTERFACE
    std::string get ## sensor ## FrameName(int sens_index) const {
        std::string frameName;
        bool ok = self->get ## sensor ## FrameName(sens_index,frameName);
        if (!ok) return "unknown";
        return frameName;
    }
#endif

#if OrientationSensor_EXTENDED_INTERFACE
    double get ## sensor ## MeasureAsRollPitchYaw(int sens_index, yarp::sig::VectorOf<double>& rpy) const {
        double timestamp;
        bool ok = self->get ## sensor ## MeasureAsRollPitchYaw(sens_index, rpy, timestamp);
#else
    double get ## sensor ## Measure(int sens_index, yarp::sig::VectorOf<double>& out) const {
        double timestamp;
        bool ok = self->get ## sensor ## Measure(sens_index, out, timestamp);
#endif
        if (!ok) return -1;
        return timestamp;
    }

#undef sensor ## _EXTENDED_INTERFACE

%enddef
