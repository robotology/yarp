/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/PidEnums.h>
#include <cstdio> // for printf
#include <cmath> //fabs
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <map>

using namespace yarp::dev;

struct PidUnits
{
    PidFeedbackUnitsEnum fbk_units;
    PidOutputUnitsEnum out_units;
    PidUnits() {
        fbk_units = PidFeedbackUnitsEnum::RAW_MACHINE_UNITS;
        out_units = PidOutputUnitsEnum::RAW_MACHINE_UNITS;
    }
};

class  yarp::dev::ControlBoardHelper::PrivateUnitsHandler
{
public:
    int nj;
    double *position_zeros;
    double *helper_ones;
    double *signs;
    int    *axisMap;
    int    *invAxisMap;
    double *angleToEncoders;
    double *newtonsToSensors;
    double *ampereToSensors;
    double *voltToSensors;
    double *dutycycleToPWMs;

    PidUnits * PosPid_units;
    PidUnits* VelPid_units;
    PidUnits* CurPid_units;
    PidUnits* TrqPid_units;
    std::map<PidControlTypeEnum, PidUnits*>   pid_units;

    explicit PrivateUnitsHandler(int size) :
        position_zeros(0),
        helper_ones(0),
        signs(0),
        axisMap(0),
        invAxisMap(0),
        angleToEncoders(0),
        newtonsToSensors(0),
        ampereToSensors(0),
        voltToSensors(0),
        dutycycleToPWMs(0),
        PosPid_units(0),
        TrqPid_units(0),
        VelPid_units(0),
        CurPid_units(0)
    {
        alloc(size);
    }

    ~PrivateUnitsHandler()
    {
        checkAndDestroy(PosPid_units);
        checkAndDestroy(VelPid_units);
        checkAndDestroy(TrqPid_units);
        checkAndDestroy(CurPid_units);
        checkAndDestroy<double>(position_zeros);
        checkAndDestroy<double>(helper_ones);
        checkAndDestroy<double>(signs);
        checkAndDestroy<int>(axisMap);
        checkAndDestroy<int>(invAxisMap);
        checkAndDestroy<double>(angleToEncoders);
        checkAndDestroy<double>(newtonsToSensors);
        checkAndDestroy<double>(ampereToSensors);
        checkAndDestroy<double>(voltToSensors);
        checkAndDestroy<double>(dutycycleToPWMs);
    }

    void alloc(int n)
    {
        nj = n;
        position_zeros = new double[nj];
        helper_ones = new double[nj];
        signs = new double[nj];
        axisMap = new int[nj];
        invAxisMap = new int[nj];
        angleToEncoders = new double[nj];
        newtonsToSensors = new double[nj];
        ampereToSensors = new double[nj];
        voltToSensors = new double[nj];
        dutycycleToPWMs = new double[nj];
        PosPid_units = new PidUnits[nj];
        VelPid_units = new PidUnits[nj];
        TrqPid_units = new PidUnits[nj];
        CurPid_units = new PidUnits[nj];

        yAssert(position_zeros != nullptr);
        yAssert(helper_ones != nullptr);
        yAssert(signs != nullptr);
        yAssert(axisMap != nullptr);
        yAssert(invAxisMap != nullptr);
        yAssert(angleToEncoders != nullptr);
        yAssert(newtonsToSensors != nullptr);
        yAssert(ampereToSensors != nullptr);
        yAssert(voltToSensors != nullptr);
        yAssert(dutycycleToPWMs != nullptr);
        yAssert(PosPid_units != nullptr);
        yAssert(VelPid_units != nullptr);
        yAssert(TrqPid_units != nullptr);
        yAssert(CurPid_units != nullptr);

        pid_units[VOCAB_PIDTYPE_POSITION] = PosPid_units;
        pid_units[VOCAB_PIDTYPE_VELOCITY] = VelPid_units;
        pid_units[VOCAB_PIDTYPE_CURRENT] = CurPid_units;
        pid_units[VOCAB_PIDTYPE_TORQUE] = TrqPid_units;
    }

    PrivateUnitsHandler(const PrivateUnitsHandler& other)
    {
        alloc(other.nj);
        memcpy(this->position_zeros, other.position_zeros, sizeof(other.position_zeros));
        memcpy(this->helper_ones, other.helper_ones, sizeof(other.helper_ones));
        memcpy(this->signs, other.signs, sizeof(other.signs));
        memcpy(this->axisMap, other.axisMap, sizeof(other.axisMap));
        memcpy(this->invAxisMap, other.invAxisMap, sizeof(other.invAxisMap));
        memcpy(this->angleToEncoders, other.angleToEncoders, sizeof(other.angleToEncoders));
        memcpy(this->newtonsToSensors, other.newtonsToSensors, sizeof(other.newtonsToSensors));
        memcpy(this->ampereToSensors, other.ampereToSensors, sizeof(other.ampereToSensors));
        memcpy(this->voltToSensors, other.voltToSensors, sizeof(other.voltToSensors));
        memcpy(this->dutycycleToPWMs, other.dutycycleToPWMs, sizeof(other.dutycycleToPWMs));
        memcpy(this->PosPid_units, other.PosPid_units, sizeof(other.PosPid_units));
        memcpy(this->VelPid_units, other.VelPid_units, sizeof(other.VelPid_units));
        memcpy(this->TrqPid_units, other.TrqPid_units, sizeof(other.TrqPid_units));
        memcpy(this->CurPid_units, other.CurPid_units, sizeof(other.CurPid_units));
    }
};

ControlBoardHelper::ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs, const double *newtons, const double *amps, const double *volts, const double *dutycycles )
{
    yAssert(n>=0);         // if number of joints is negative complain!
    yAssert(aMap!=0);      // at least the axisMap is required
    mPriv = new PrivateUnitsHandler(n);

    memcpy(mPriv->axisMap, aMap, sizeof(int)*n);

    std::fill_n(mPriv->helper_ones, n, 1.0);

    if (zs!=0)
        memcpy(mPriv->position_zeros, zs, sizeof(double)*n);
    else
        std::fill_n(mPriv->position_zeros, n, 0.0);

    if (angToEncs!=0)
        memcpy(mPriv->angleToEncoders, angToEncs, sizeof(double)*n);
    else
        std::fill_n(mPriv->angleToEncoders, n, 1.0);

    if (newtons!=0)
        memcpy(mPriv->newtonsToSensors, newtons, sizeof(double)*n);
    else
        std::fill_n(mPriv->newtonsToSensors, n, 1.0);

    if (amps!=0)
        memcpy(mPriv->ampereToSensors, amps, sizeof(double)*n);
    else
        std::fill_n(mPriv->ampereToSensors, n, 1.0);

    if (volts!=0)
        memcpy(mPriv->voltToSensors, volts, sizeof(double)*n);
    else
        std::fill_n(mPriv->voltToSensors, n, 1.0);

    if (dutycycles != 0)
        memcpy(mPriv->dutycycleToPWMs, dutycycles, sizeof(double)*n);
    else
        std::fill_n(mPriv->dutycycleToPWMs, n, 1.0);

    // invert the axis map
    memset (mPriv->invAxisMap, 0, sizeof(int) * n);
    int i;
    for (i = 0; i < n; i++)
    {
        int j;
        for (j = 0; j < n; j++)
        {
            if (mPriv->axisMap[j] == i)
            {
                mPriv->invAxisMap[i] = j;
                break;
            }
        }
    }
}

ControlBoardHelper::~ControlBoardHelper()
{
    if (mPriv) { delete mPriv; mPriv = 0; }
}

yarp::dev::ControlBoardHelper::ControlBoardHelper(const ControlBoardHelper & other)
{
    mPriv = new PrivateUnitsHandler(*other.mPriv);
}

ControlBoardHelper& ControlBoardHelper::operator = (const ControlBoardHelper & other)
{
    mPriv = new PrivateUnitsHandler(*other.mPriv);
    return *this;
}

bool ControlBoardHelper::checkAxisId(int id)
{
    if (id >= mPriv->nj)
    {
        return false;
    }
    return true;
}

int ControlBoardHelper::toHw(int axis)
{ return mPriv->axisMap[axis]; }

int ControlBoardHelper::toUser(int axis)
{ return mPriv->invAxisMap[axis]; }

//map a vector, no conversion
    void ControlBoardHelper::toUser(const double *hwData, double *user)
{
    for (int k=0;k<mPriv->nj;k++)
        user[toUser(k)]=hwData[k];
}

//map a vector, no conversion
void ControlBoardHelper::ControlBoardHelper::toUser(const int *hwData, int *user)
{
    for (int k=0;k<mPriv->nj;k++)
        user[toUser(k)]=hwData[k];
}

//map a vector, no conversion
    void ControlBoardHelper::toHw(const double *usr, double *hwData)
{
    for (int k=0;k<mPriv->nj;k++)
        hwData[toHw(k)]=usr[k];
}

//map a vector, no conversion
void ControlBoardHelper::toHw(const int *usr, int *hwData)
{
    for (int k=0;k<mPriv->nj;k++)
        hwData[toHw(k)]=usr[k];
}

void ControlBoardHelper::posA2E(double ang, int j, double &enc, int &k)
{
    enc=(ang+ mPriv->position_zeros[j])*mPriv->angleToEncoders[j];
    k=toHw(j);
}

double ControlBoardHelper::posA2E(double ang, int j)
{
    return (ang+ mPriv->position_zeros[j])*mPriv->angleToEncoders[j];
}

void ControlBoardHelper::posE2A(double enc, int j, double &ang, int &k)
{
    k=toUser(j);

    ang=(enc/ mPriv->angleToEncoders[k])- mPriv->position_zeros[k];
}

double ControlBoardHelper::posE2A(double enc, int j)
{
    int k=toUser(j);

    return (enc/ mPriv->angleToEncoders[k])- mPriv->position_zeros[k];
}

void ControlBoardHelper::impN2S(double newtons, int j, double &sens, int &k)
{
    sens=newtons* mPriv->newtonsToSensors[j]/ mPriv->angleToEncoders[j];
    k=toHw(j);
}

double ControlBoardHelper::impN2S(double newtons, int j)
{
    return newtons* mPriv->newtonsToSensors[j]/ mPriv->angleToEncoders[j];
}

void ControlBoardHelper::impN2S(const double *newtons, double *sens)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        impN2S(newtons[j], j, tmp, index);
        sens[index]=tmp;
    }
}

void ControlBoardHelper::trqN2S(double newtons, int j, double &sens, int &k)
{
    sens=newtons* mPriv->newtonsToSensors[j];
    k=toHw(j);
}

double ControlBoardHelper::trqN2S(double newtons, int j)
{
    return newtons* mPriv->newtonsToSensors[j];
}

//map a vector, convert from newtons to sensors
void ControlBoardHelper::trqN2S(const double *newtons, double *sens)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        trqN2S(newtons[j], j, tmp, index);
        sens[index]=tmp;
    }
}

//map a vector, convert from sensor to newtons
void ControlBoardHelper::trqS2N(const double *sens, double *newtons)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        trqS2N(sens[j], j, tmp, index);
        newtons[index]=tmp;
    }
}

void ControlBoardHelper::trqS2N(double sens, int j, double &newton, int &k)
{
    k=toUser(j);
    newton=(sens/ mPriv->newtonsToSensors[k]);
}

double ControlBoardHelper::trqS2N(double sens, int j)
{
    int k=toUser(j);
    return (sens/ mPriv->newtonsToSensors[k]);
}

void ControlBoardHelper::impS2N(const double *sens, double *newtons)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        impS2N(sens[j], j, tmp, index);
        newtons[index]=tmp;
    }
}

void ControlBoardHelper::impS2N(double sens, int j, double &newton, int &k)
{
    k=toUser(j);
    newton=(sens/ mPriv->newtonsToSensors[k]* mPriv->angleToEncoders[k]);
}

double ControlBoardHelper::impS2N(double sens, int j)
{
    int k=toUser(j);

    return (sens/ mPriv->newtonsToSensors[k]* mPriv->angleToEncoders[k]);
}

void ControlBoardHelper::velA2E(double ang, int j, double &enc, int &k)
{
    k=toHw(j);
    enc=ang* mPriv->angleToEncoders[j];
}

double ControlBoardHelper::velA2E(double ang, int j)
{
    return ang* mPriv->angleToEncoders[j];
}

void ControlBoardHelper::velA2E_abs(double ang, int j, double &enc, int &k)
{
    k=toHw(j);
    enc=ang*fabs(mPriv->angleToEncoders[j]);
}

void ControlBoardHelper::velE2A(double enc, int j, double &ang, int &k)
{
    k=toUser(j);
    ang=enc/ mPriv->angleToEncoders[k];
}

void ControlBoardHelper::velE2A_abs(double enc, int j, double &ang, int &k)
{
    k=toUser(j);
    ang=enc/fabs(mPriv->angleToEncoders[k]);
}

void ControlBoardHelper::accA2E(double ang, int j, double &enc, int &k)
{
    velA2E(ang, j, enc, k);
}

void ControlBoardHelper::accA2E_abs(double ang, int j, double &enc, int &k)
{
    velA2E_abs(ang, j, enc, k);
}

void ControlBoardHelper::accE2A(double enc, int j, double &ang, int &k)
{
    velE2A(enc, j, ang, k);
}

void ControlBoardHelper::accE2A_abs(double enc, int j, double &ang, int &k)
{
    velE2A_abs(enc, j, ang, k);
}

double ControlBoardHelper::velE2A(double enc, int j)
{
    int k=toUser(j);
    return enc/ mPriv->angleToEncoders[k];
}

double ControlBoardHelper::velE2A_abs(double enc, int j)
{
    int k=toUser(j);
    return enc/fabs(mPriv->angleToEncoders[k]);
}


double ControlBoardHelper::accE2A(double enc, int j)
{
    return velE2A(enc, j);
}

double ControlBoardHelper::accE2A_abs(double enc, int j)
{
    return velE2A_abs(enc, j);
}

//map a vector, convert from angles to encoders
void ControlBoardHelper::posA2E(const double *ang, double *enc)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        posA2E(ang[j], j, tmp, index);
        enc[index]=tmp;
    }
}

//map a vector, convert from encoders to angles
void ControlBoardHelper::posE2A(const double *enc, double *ang)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        posE2A(enc[j], j, tmp, index);
        ang[index]=tmp;
    }
}

void ControlBoardHelper::velA2E(const double *ang, double *enc)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        velA2E(ang[j], j, tmp, index);
        enc[index]=tmp;
    }
}

void ControlBoardHelper::velA2E_abs(const double *ang, double *enc)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        velA2E_abs(ang[j], j, tmp, index);
        enc[index]=tmp;
    }
}

void ControlBoardHelper::velE2A(const double *enc, double *ang)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        velE2A(enc[j], j, tmp, index);
        ang[index]=tmp;
    }
}

void ControlBoardHelper::velE2A_abs(const double *enc, double *ang)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        velE2A_abs(enc[j], j, tmp, index);
        ang[index]=tmp;
    }
}

void ControlBoardHelper::accA2E(const double *ang, double *enc)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        accA2E(ang[j], j, tmp, index);
        enc[index]=tmp;
    }
}

void ControlBoardHelper::accA2E_abs(const double *ang, double *enc)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        accA2E_abs(ang[j], j, tmp, index);
        enc[index]=tmp;
    }
}

void ControlBoardHelper::accE2A(const double *enc, double *ang)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        accE2A(enc[j], j, tmp, index);
        ang[index]=tmp;
    }
}

void ControlBoardHelper::accE2A_abs(const double *enc, double *ang)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        accE2A_abs(enc[j], j, tmp, index);
        ang[index]=tmp;
    }
}

//***************** current ******************//
void ControlBoardHelper::ampereA2S(double ampere, int j, double &sens, int &k)
{
    sens=ampere* mPriv->ampereToSensors[j];
    k=toHw(j);
}

double ControlBoardHelper::ampereA2S(double ampere, int j)
{
        return ampere* mPriv->ampereToSensors[j];
}

//map a vector, convert from ampere to sensors
void ControlBoardHelper::ampereA2S(const double *ampere, double *sens)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        ampereA2S(ampere[j], j, tmp, index);
        sens[index]=tmp;
    }
}

//map a vector, convert from sensor to ampere
void ControlBoardHelper::ampereS2A(const double *sens, double *ampere)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        ampereS2A(sens[j], j, tmp, index);
        ampere[index]=tmp;
    }
}

void ControlBoardHelper::ampereS2A(double sens, int j, double &ampere, int &k)
{
    k=toUser(j);
    ampere=(sens/ mPriv->ampereToSensors[k]);
}

double ControlBoardHelper::ampereS2A(double sens, int j)
{
    int k=toUser(j);
    return sens/ mPriv->ampereToSensors[k];
}
// *******************************************//

//***************** voltage ******************//
void ControlBoardHelper::voltageV2S(double voltage, int j, double &sens, int &k)
{
    sens=voltage* mPriv->voltToSensors[j];
    k=toHw(j);
}

double ControlBoardHelper::voltageV2S(double voltage, int j)
{
    return voltage* mPriv->voltToSensors[j];
}

//map a vector, convert from voltage to sensors
void ControlBoardHelper::voltageV2S(const double *voltage, double *sens)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        voltageV2S(voltage[j], j, tmp, index);
        sens[index]=tmp;
    }
}

//map a vector, convert from sensor to newtons
void ControlBoardHelper::voltageS2V(const double *sens, double *voltage)
{
    double tmp;
    int index;
    for(int j=0;j<mPriv->nj;j++)
    {
        voltageS2V(sens[j], j, tmp, index);
        voltage[index]=tmp;
    }
}

void ControlBoardHelper::voltageS2V(double sens, int j, double &voltage, int &k)
{
    k=toUser(j);
    voltage=(sens/ mPriv->voltToSensors[k]);
}

double ControlBoardHelper::voltageS2V(double sens, int j)
{
    int k=toUser(j);
    return (sens/ mPriv->voltToSensors[k]);
}
// *******************************************//

//***************** dutycycle ******************//
void ControlBoardHelper::dutycycle2PWM(double dutycycle, int j, double &pwm, int &k)
{
    pwm = dutycycle* mPriv->dutycycleToPWMs[j];
    k = toHw(j);
}

double ControlBoardHelper::dutycycle2PWM(double dutycycle, int j)
{
    return dutycycle* mPriv->dutycycleToPWMs[j];
}

void ControlBoardHelper::dutycycle2PWM(const double *dutycycle, double *sens)
{
    double tmp;
    int index;
    for (int j = 0; j<mPriv->nj; j++)
    {
        dutycycle2PWM(dutycycle[j], j, tmp, index);
        sens[index] = tmp;
    }
}

void ControlBoardHelper::PWM2dutycycle(const double *pwm, double *dutycycle)
{
    double tmp;
    int index;
    for (int j = 0; j<mPriv->nj; j++)
    {
        PWM2dutycycle(pwm[j], j, tmp, index);
        dutycycle[index] = tmp;
    }
}

void ControlBoardHelper::PWM2dutycycle(double pwm_raw, int k_raw, double &dutycycle, int &j)
{
    j = toUser(k_raw);
    dutycycle = (pwm_raw / mPriv->dutycycleToPWMs[j]);
}

double ControlBoardHelper::PWM2dutycycle(double pwm_raw, int k_raw)
{
    int j = toUser(k_raw);
    return (pwm_raw / mPriv->dutycycleToPWMs[j]);
}

// *******************************************//
double ControlBoardHelper::raw2user(double value_raw, int k_raw, const double* conv_user2raw)
{
    int j = toUser(k_raw);
    return (value_raw / conv_user2raw[j]);
}

double ControlBoardHelper::user2raw(double value_user, int j, const double* conv_user2raw)
{
    return value_user * conv_user2raw[j];
}

void ControlBoardHelper::user2raw(double value_user, int j, double &value_raw, int &k, const double* conv_user2raw)
{
    value_raw = value_user * conv_user2raw[j];
    k = toHw(j);
}

void ControlBoardHelper::raw2user(double value_raw, int k_raw, double &value_user, int &j, const double* conv_user2raw)
{
    j = toUser(k_raw);
    value_user = (value_raw / conv_user2raw[j]);
}

double ControlBoardHelper::raw2user(double value_raw, int k_raw)
{
    int j = toUser(k_raw);
    return (value_raw / mPriv->dutycycleToPWMs[j]);
}

void ControlBoardHelper::bemfuser2bemfraw(double bemf_user, int j, double &bemf_raw, int &k)
{
    bemf_raw = bemf_user * mPriv->newtonsToSensors[j] / mPriv->angleToEncoders[j];
    k = toHw(j);
}

void ControlBoardHelper::ktauuser2ktauraw(double ktau_user, int j, double &ktau_raw, int &k)
{
    ktau_raw = ktau_user * mPriv->dutycycleToPWMs[j] / mPriv->newtonsToSensors[j];
    k = toHw(j);
}

void ControlBoardHelper::bemfraw2bemfuser(double bemf_raw, int k_raw, double &bemf_user, int &j_user)
{
    j_user = toUser(k_raw);
    bemf_user = bemf_raw / mPriv->newtonsToSensors[j_user] * mPriv->angleToEncoders[j_user];
}

void ControlBoardHelper::ktauraw2ktauuser(double ktau_raw, int k_raw, double &ktau_user, int &j_user)
{
    j_user = toUser(k_raw);
    ktau_user = ktau_raw / mPriv->dutycycleToPWMs[j_user] * mPriv->newtonsToSensors[j_user];
}

double  ControlBoardHelper::bemfuser2bemfraw(double bemf_user, int j)
{
    return bemf_user * mPriv->newtonsToSensors[j] / mPriv->angleToEncoders[j];
}

double  ControlBoardHelper::ktauuser2ktauraw(double ktau_user, int j)
{
    return ktau_user * mPriv->dutycycleToPWMs[j] / mPriv->newtonsToSensors[j];;
}

// *******************************************//

int ControlBoardHelper::axes()
{ return mPriv->nj; }

// *******************************************//

void ControlBoardHelper::get_pidoutput_conversion_units(const yarp::dev::PidControlTypeEnum& pidtype, int j, double*&  output_conversion_units)
{
    switch (mPriv->pid_units[pidtype][j].out_units)
    {
    case  PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT:  output_conversion_units = mPriv->dutycycleToPWMs; break;
    case  PidOutputUnitsEnum::CURRENT_METRIC:  output_conversion_units = mPriv->ampereToSensors; break;
    case  PidOutputUnitsEnum::POSITION_METRIC:  output_conversion_units = mPriv->angleToEncoders; break;
    case  PidOutputUnitsEnum::VELOCITY_METRIC:  output_conversion_units = mPriv->angleToEncoders; break;
    case  PidOutputUnitsEnum::TORQUE_METRIC:  output_conversion_units = mPriv->newtonsToSensors; break;
    default: output_conversion_units = mPriv->helper_ones; break;
    }
}

void ControlBoardHelper::convert_pid_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_raw, int j_raw, Pid &out_usr, int &k_usr)
{
    double* output_conversion_units = 0;
    ControlBoardHelper* cb_helper = this;
    k_usr = cb_helper->toUser(j_raw);
    out_usr = in_raw;

    switch (pidtype)
    {
    case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION:
        if (mPriv->pid_units[VOCAB_PIDTYPE_POSITION][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->posE2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->posE2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->posE2A(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY:
        if (mPriv->pid_units[VOCAB_PIDTYPE_VELOCITY][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->velE2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->velE2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->velE2A(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE:
        if (mPriv->pid_units[VOCAB_PIDTYPE_TORQUE][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->trqS2N(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->trqS2N(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->trqS2N(out_usr.kd, j_raw);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT:
        if (mPriv->pid_units[VOCAB_PIDTYPE_CURRENT][k_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_usr.kp = cb_helper->ampereS2A(out_usr.kp, j_raw);
            out_usr.ki = cb_helper->ampereS2A(out_usr.ki, j_raw);
            out_usr.kd = cb_helper->ampereS2A(out_usr.kd, j_raw);
        }
        break;
    default:
        break;
    }

    get_pidoutput_conversion_units(pidtype, j_raw, output_conversion_units);
    out_usr.kp = cb_helper->raw2user(out_usr.kp, j_raw, output_conversion_units);
    out_usr.ki = cb_helper->raw2user(out_usr.ki, j_raw, output_conversion_units);
    out_usr.kd = cb_helper->raw2user(out_usr.kd, j_raw, output_conversion_units);
    out_usr.max_output = cb_helper->raw2user(out_usr.max_output, j_raw, output_conversion_units);
    out_usr.max_int = cb_helper->raw2user(out_usr.max_int, j_raw, output_conversion_units);
    out_usr.stiction_up_val = cb_helper->raw2user(out_usr.stiction_up_val, j_raw, output_conversion_units);
    out_usr.stiction_down_val = cb_helper->raw2user(out_usr.stiction_down_val, j_raw, output_conversion_units);
    out_usr.offset = cb_helper->raw2user(out_usr.offset, j_raw, output_conversion_units);
}

Pid ControlBoardHelper::convert_pid_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr)
{
    Pid tmp;
    int tmp_k;
    convert_pid_to_machine(pidtype, in_usr, j_usr, tmp, tmp_k);
    return tmp;
}

void ControlBoardHelper::convert_pid_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_usr, int j_usr, Pid &out_raw, int &k_raw)
{
    ControlBoardHelper* cb_helper = this;
    double* output_conversion_units = 0;
    k_raw = cb_helper->toHw(j_usr);
    out_raw = in_usr;

    switch (pidtype)
    {
    case PidControlTypeEnum::VOCAB_PIDTYPE_POSITION:
        if (mPriv->pid_units[VOCAB_PIDTYPE_POSITION][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = cb_helper->posA2E(out_raw.kp, j_usr);
            out_raw.ki = cb_helper->posA2E(out_raw.ki, j_usr);
            out_raw.kd = cb_helper->posA2E(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY:
        if (mPriv->pid_units[VOCAB_PIDTYPE_VELOCITY][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = cb_helper->velA2E(out_raw.kp, j_usr);
            out_raw.ki = cb_helper->velA2E(out_raw.ki, j_usr);
            out_raw.kd = cb_helper->velA2E(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE:
        if (mPriv->pid_units[VOCAB_PIDTYPE_TORQUE][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = cb_helper->trqN2S(out_raw.kp, j_usr);
            out_raw.ki = cb_helper->trqN2S(out_raw.ki, j_usr);
            out_raw.kd = cb_helper->trqN2S(out_raw.kd, j_usr);
        }
        break;
    case PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT:
        if (mPriv->pid_units[VOCAB_PIDTYPE_CURRENT][j_usr].fbk_units == PidFeedbackUnitsEnum::METRIC)
        {
            out_raw.kp = cb_helper->ampereA2S(out_raw.kp, j_usr);
            out_raw.ki = cb_helper->ampereA2S(out_raw.ki, j_usr);
            out_raw.kd = cb_helper->ampereA2S(out_raw.kd, j_usr);
        }
        break;
    default:
        break;
    }

    get_pidoutput_conversion_units(pidtype, j_usr, output_conversion_units);
    out_raw.kp = cb_helper->user2raw(out_raw.kp, j_usr, output_conversion_units);
    out_raw.ki = cb_helper->user2raw(out_raw.ki, j_usr, output_conversion_units);
    out_raw.kd = cb_helper->user2raw(out_raw.kd, j_usr, output_conversion_units);
    out_raw.max_output = cb_helper->user2raw(out_raw.max_output, j_usr, output_conversion_units);
    out_raw.max_int = cb_helper->user2raw(out_raw.max_int, j_usr, output_conversion_units);
    out_raw.stiction_up_val = cb_helper->user2raw(out_raw.stiction_up_val, j_usr, output_conversion_units);
    out_raw.stiction_down_val = cb_helper->user2raw(out_raw.stiction_down_val, j_usr, output_conversion_units);
    out_raw.offset = cb_helper->user2raw(out_raw.offset, j_usr, output_conversion_units);
}

void ControlBoardHelper::convert_pidunits_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k)
{
    ControlBoardHelper* cb_helper = this;
    switch (pidtype)
    {
    case yarp::dev::VOCAB_PIDTYPE_POSITION:
        cb_helper->posA2E(userval, j, machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
        cb_helper->velA2E(userval, j, machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_TORQUE:
        cb_helper->trqN2S(userval, j, machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_CURRENT:
        cb_helper->ampereA2S(userval, j, machineval, k);
        break;
    default:
        yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ControlBoardHelper::convert_pidunits_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, const double* userval, double* machineval)
{
    ControlBoardHelper* cb_helper = this;
    switch (pidtype)
    {
    case yarp::dev::VOCAB_PIDTYPE_POSITION:
        cb_helper->posA2E(userval, machineval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
        cb_helper->velA2E(userval, machineval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_TORQUE:
        cb_helper->trqN2S(userval, machineval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_CURRENT:
        cb_helper->ampereA2S(userval, machineval);
        break;
    default:
        yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ControlBoardHelper::convert_pidunits_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double machineval, double* userval, int k)
{
    ControlBoardHelper* cb_helper = this;
    switch (pidtype)
    {
    case yarp::dev::VOCAB_PIDTYPE_POSITION:
        *userval = cb_helper->posE2A(machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
        *userval = cb_helper->velE2A(machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_TORQUE:
        *userval = cb_helper->trqS2N(machineval, k);
        break;
    case yarp::dev::VOCAB_PIDTYPE_CURRENT:
        *userval = cb_helper->ampereS2A(machineval, k);
        break;
    default:
        yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ControlBoardHelper::convert_pidunits_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const double* machineval, double* userval)
{
    ControlBoardHelper* cb_helper = this;
    switch (pidtype)
    {
    case yarp::dev::VOCAB_PIDTYPE_POSITION:
        cb_helper->posE2A(machineval, userval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_VELOCITY:
        cb_helper->velE2A(machineval, userval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_TORQUE:
        cb_helper->trqS2N(machineval, userval);
        break;
    case yarp::dev::VOCAB_PIDTYPE_CURRENT:
        cb_helper->ampereS2A(machineval, userval);
        break;
    default:
        yError() << "convert_units_to_machine: invalid pidtype";
        break;
    }
}

void ControlBoardHelper::set_pid_conversion_units(const PidControlTypeEnum& pidtype, const PidFeedbackUnitsEnum fbk_conv_units, const PidOutputUnitsEnum out_conv_units)
{
    ControlBoardHelper* cb_helper = this;
    int nj = cb_helper->axes();
    for (size_t i = 0; i < nj; i++)
    {
        mPriv->pid_units[pidtype][i].fbk_units = fbk_conv_units;
        mPriv->pid_units[pidtype][i].out_units = out_conv_units;
    }
}
