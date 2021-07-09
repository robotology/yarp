/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_CONTROLBOARDHELPER_H
#define YARP_DEV_CONTROLBOARDHELPER_H

#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/PidEnums.h>
#include <yarp/os/Log.h>

#include <cstring> // for memset
#include <type_traits>

 /*
 * simple helper template to alloc memory.
 */
template <class T,
          std::enable_if_t<std::is_trivial<T>::value, int> = 0>
T* allocAndCheck(int size)
{
    T* t = new T[size];
    yAssert(t != 0);
    memset(t, 0, sizeof(T) * size);
    return t;
}

template <class T,
          std::enable_if_t<!std::is_trivial<T>::value, int> = 0>
T* allocAndCheck(int size)
{
    T* t = new T[size];
    yAssert(t != 0);
    return t;
}

/*
*
*/
template <class T>
void checkAndDestroy(T* &p) {
    if (p != 0) {
        delete[] p;
        p = 0;
    }
}

namespace yarp {
    namespace dev {
        class ControlBoardHelper;
    }
}

//////////////// Control Board Helper
class YARP_dev_API yarp::dev::ControlBoardHelper
{
public:
    ControlBoardHelper(int n, const int *aMap, const double *angToEncs = nullptr, const double *zs = nullptr, const double *newtons = nullptr, const double *amps = nullptr, const double *volts = nullptr, const double *dutycycles = nullptr, const double *kbemf = nullptr, const double *ktau = nullptr);
    ~ControlBoardHelper();
    ControlBoardHelper(const ControlBoardHelper& other);
    ControlBoardHelper& operator = (const ControlBoardHelper & other);

public:
    bool checkAxisId(int id);
    bool checkAxesIds(const int n_axes, const int* axesList);
    int toHw(int axis);
    int toUser(int axis);
    void toUser(const double *hwData, double *user);
    void toUser(const int *hwData, int *user);
    void toHw(const double *usr, double *hwData);
    void toHw(const int *usr, int *hwData);

    //***************** position ******************//
    void posA2E(double ang, int j, double &enc, int &k);
    double posA2E(double ang, int j);
    void posE2A(double enc, int j, double &ang, int &k);
    double posE2A(double enc, int j);
    void posA2E(const double *ang, double *enc);
    void posE2A(const double *enc, double *ang);

    //***************** velocity  ******************//
    void velA2E(double ang, int j, double &enc, int &k);
    double velA2E(double ang, int j);
    void velA2E_abs(double ang, int j, double &enc, int &k);
    void velE2A(double enc, int j, double &ang, int &k);
    void velE2A_abs(double enc, int j, double &ang, int &k);
    double velE2A(double enc, int j);
    double velE2A_abs(double enc, int j);
    void velA2E(const double *ang, double *enc);
    void velA2E_abs(const double *ang, double *enc);
    void velE2A(const double *enc, double *ang);
    void velE2A_abs(const double *enc, double *ang);

    //***************** acceleration  ******************//
    void accA2E(double ang, int j, double &enc, int &k);
    void accA2E_abs(double ang, int j, double &enc, int &k);
    void accE2A(double enc, int j, double &ang, int &k);
    void accE2A_abs(double enc, int j, double &ang, int &k);
    double accE2A(double enc, int j);
    double accE2A_abs(double enc, int j);
    void accA2E(const double *ang, double *enc);
    void accA2E_abs(const double *ang, double *enc);
    void accE2A(const double *enc, double *ang);
    void accE2A_abs(const double *enc, double *ang);

    //***************** torque ******************//
    void trqN2S(double newtons, int j, double &sens, int &k);
    double trqN2S(double newtons, int j);
    void trqN2S(const double *newtons, double *sens);
    void trqS2N(const double *sens, double *newtons);
    void trqS2N(double sens, int j, double &newton, int &k);
    double trqS2N(double sens, int j);

    //***************** impedance ******************//
    void impN2S(double newtons, int j, double &sens, int &k);
    double impN2S(double newtons, int j);
    void impN2S(const double *newtons, double *sens);
    void impS2N(const double *sens, double *newtons);
    void impS2N(double sens, int j, double &newton, int &k);
    double impS2N(double sens, int j);

    //***************** current ******************//
    void ampereA2S(double ampere, int j, double &sens, int &k);
    double ampereA2S(double ampere, int j);
    void ampereA2S(const double *ampere, double *sens);
    void ampereS2A(const double *sens, double *ampere);
    void ampereS2A(double sens, int j, double &ampere, int &k);
    double ampereS2A(double sens, int j);

    //***************** voltage ******************//
    void voltageV2S(double voltage, int j, double &sens, int &k);
    double voltageV2S(double voltage, int j);
    void voltageV2S(const double *voltage, double *sens);
    void voltageS2V(const double *sens, double *voltage);
    void voltageS2V(double sens, int j, double &voltage, int &k);
    double voltageS2V(double sens, int j);

    //***************** dutycycle ******************//
    void dutycycle2PWM(double dutycycle, int j, double &pwm, int &k);
    double dutycycle2PWM(double dutycycle, int j);
    void dutycycle2PWM(const double *dutycycle, double *sens);
    void PWM2dutycycle(const double *pwm, double *dutycycle);
    void PWM2dutycycle(double pwm_raw, int k_raw, double &dutycycle, int &j);
    double PWM2dutycycle(double pwm_raw, int k_raw);

    // *******************************************//
    double bemf_user2raw(double bemf_user, int j);
    double ktau_user2raw(double ktau_user, int j);
    void bemf_user2raw(double bemf_user, int j, double &bemf_raw, int &k);
    void ktau_user2raw(double ktau_user, int j, double &ktau_raw, int &k);
    void bemf_raw2user(double bemf_raw, int k_raw, double &bemf_user, int &j_user);
    void ktau_raw2user(double ktau_raw, int k_raw, double &ktau_user, int &j_user);

    int axes();

    // *******************************************//
public:
    void convert_pidunits_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k);
    void convert_pidunits_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const double* userval, double* machineval);
    void convert_pidunits_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double machineval, double* userval, int k);
    void convert_pidunits_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double* machineval, double* userval);

    void convert_pid_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_raw, int j_raw, Pid &out_usr, int &k_usr);
    void convert_pid_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr, Pid &out_raw, int &k_raw);
    Pid  convert_pid_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr);

    void set_pid_conversion_units(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units);
    double get_pidfeedback_conversion_factor_user2raw(const yarp::dev::PidControlTypeEnum& pidtype,  int j);
    double get_pidoutput_conversion_factor_user2raw(const yarp::dev::PidControlTypeEnum& pidtype, int j);

private:
    class PrivateUnitsHandler;
    PrivateUnitsHandler* mPriv;
};

inline yarp::dev::ControlBoardHelper *castToMapper(void *p)
{ return static_cast<yarp::dev::ControlBoardHelper *>(p); }

#endif // YARP_DEV_CONTROLBOARDHELPER_H
