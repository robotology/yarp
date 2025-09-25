/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
    int     nj;
    int    *axisMap;
    int    *invAxisMap;
    bool    verbose;

    double *position_zeros;
    double *helper_ones;
    double *angleToEncoders;
    double *newtonsToSensors;
    double *ampereToSensors;
    double *voltToSensors;
    double *dutycycleToPWMs;
    double *bemfToRaws;
    double *ktauToRaws;
    double *viscousPosToRaws;
    double *viscousNegToRaws;
    double *coulombPosToRaws;
    double *coulombNegToRaws;
    double *velocityThresToRaws;

    PidUnits* PosPid_units = nullptr;
    PidUnits* PosDirPid_units = nullptr;
    PidUnits* VelPid_units = nullptr;
    PidUnits* VelDirPid_units = nullptr;
    PidUnits* CurPid_units = nullptr;
    PidUnits* TrqPid_units = nullptr;
    PidUnits* MixPid_units = nullptr;
    std::map<PidControlTypeEnum, PidUnits*>   pid_units;

    explicit PrivateUnitsHandler(int size) :
        axisMap(nullptr),
        invAxisMap(nullptr),
        verbose(true),
        position_zeros(nullptr),
        helper_ones(nullptr),
        angleToEncoders(nullptr),
        newtonsToSensors(nullptr),
        ampereToSensors(nullptr),
        voltToSensors(nullptr),
        dutycycleToPWMs(nullptr),
        bemfToRaws(nullptr),
        ktauToRaws(nullptr),
        viscousPosToRaws(nullptr),
        viscousNegToRaws(nullptr),
        coulombPosToRaws(nullptr),
        coulombNegToRaws(nullptr),
        velocityThresToRaws(nullptr)
    {
        alloc(size);
        std::fill_n(helper_ones, size, 1.0);
        std::fill_n(position_zeros, size, 0.0);
        std::fill_n(angleToEncoders, size, 1.0);
        std::fill_n(newtonsToSensors, size, 1.0);
        std::fill_n(ampereToSensors, size, 1.0);
        std::fill_n(voltToSensors, size, 1.0);
        std::fill_n(dutycycleToPWMs, size, 1.0);
        std::fill_n(bemfToRaws, size, 1.0);
        std::fill_n(ktauToRaws, size, 1.0);
        std::fill_n(viscousPosToRaws, size, 1.0);
        std::fill_n(viscousNegToRaws, size, 1.0);
        std::fill_n(coulombPosToRaws, size, 1.0);
        std::fill_n(coulombNegToRaws, size, 1.0);
        std::fill_n(velocityThresToRaws, size, 1.0);
    }

    ~PrivateUnitsHandler()
    {
        checkAndDestroy(PosPid_units);
        checkAndDestroy(VelPid_units);
        checkAndDestroy(TrqPid_units);
        checkAndDestroy(CurPid_units);
        checkAndDestroy(PosDirPid_units);
        checkAndDestroy(VelDirPid_units);
        checkAndDestroy(MixPid_units);
        checkAndDestroy<double>(position_zeros);
        checkAndDestroy<double>(helper_ones);
        checkAndDestroy<int>(axisMap);
        checkAndDestroy<int>(invAxisMap);
        checkAndDestroy<double>(angleToEncoders);
        checkAndDestroy<double>(newtonsToSensors);
        checkAndDestroy<double>(ampereToSensors);
        checkAndDestroy<double>(voltToSensors);
        checkAndDestroy<double>(dutycycleToPWMs);
        checkAndDestroy<double>(bemfToRaws);
        checkAndDestroy<double>(ktauToRaws);
        checkAndDestroy<double>(viscousPosToRaws);
        checkAndDestroy<double>(viscousNegToRaws);
        checkAndDestroy<double>(coulombPosToRaws);
        checkAndDestroy<double>(coulombNegToRaws);
        checkAndDestroy<double>(velocityThresToRaws);
    }

    void alloc(int n)
    {
        nj = n;
        position_zeros = new double[nj];
        helper_ones = new double[nj];
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
        PosDirPid_units = new PidUnits[nj];
        VelDirPid_units = new PidUnits[nj];
        MixPid_units = new PidUnits[nj];
        bemfToRaws = new double[nj];
        ktauToRaws = new double[nj];
        viscousPosToRaws = new double[nj];
        viscousNegToRaws = new double[nj];
        coulombPosToRaws = new double[nj];
        coulombNegToRaws = new double[nj];
        velocityThresToRaws = new double[nj];

        yAssert(position_zeros != nullptr);
        yAssert(helper_ones != nullptr);
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
        yAssert(PosDirPid_units != nullptr);
        yAssert(VelDirPid_units != nullptr);
        yAssert(MixPid_units != nullptr);
        yAssert(bemfToRaws != nullptr);
        yAssert(ktauToRaws != nullptr);
        yAssert(viscousPosToRaws != nullptr);
        yAssert(viscousNegToRaws != nullptr);
        yAssert(coulombPosToRaws != nullptr);
        yAssert(coulombNegToRaws != nullptr);
        yAssert(velocityThresToRaws != nullptr);

        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1] = PosPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2] = PosPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3] = PosPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1] = VelPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2] = VelPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3] = VelPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1] = CurPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2] = CurPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3] = CurPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1] = TrqPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2] = TrqPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3] = TrqPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1] = PosDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2] = PosDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3] = PosDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1] = VelDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2] = VelDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3] = VelDirPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1] = MixPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2] = MixPid_units;
        pid_units[PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3] = MixPid_units;
    }

    PrivateUnitsHandler(const PrivateUnitsHandler& other)
    {
        alloc(other.nj);
        memcpy(this->position_zeros, other.position_zeros, sizeof(*other.position_zeros)*nj);
        memcpy(this->helper_ones, other.helper_ones, sizeof(*other.helper_ones)*nj);
        memcpy(this->axisMap, other.axisMap, sizeof(*other.axisMap)*nj);
        memcpy(this->invAxisMap, other.invAxisMap, sizeof(*other.invAxisMap)*nj);
        memcpy(this->angleToEncoders, other.angleToEncoders, sizeof(*other.angleToEncoders)*nj);
        memcpy(this->newtonsToSensors, other.newtonsToSensors, sizeof(*other.newtonsToSensors)*nj);
        memcpy(this->ampereToSensors, other.ampereToSensors, sizeof(*other.ampereToSensors)*nj);
        memcpy(this->voltToSensors, other.voltToSensors, sizeof(*other.voltToSensors)*nj);
        memcpy(this->dutycycleToPWMs, other.dutycycleToPWMs, sizeof(*other.dutycycleToPWMs)*nj);
        memcpy(this->PosPid_units, other.PosPid_units, sizeof(*other.PosPid_units)*nj);
        memcpy(this->VelPid_units, other.VelPid_units, sizeof(*other.VelPid_units)*nj);
        memcpy(this->TrqPid_units, other.TrqPid_units, sizeof(*other.TrqPid_units)*nj);
        memcpy(this->CurPid_units, other.CurPid_units, sizeof(*other.CurPid_units)*nj);
        memcpy(this->PosDirPid_units, other.PosDirPid_units, sizeof(*other.PosDirPid_units)*nj);
        memcpy(this->VelDirPid_units, other.VelDirPid_units, sizeof(*other.VelDirPid_units)*nj);
        memcpy(this->MixPid_units, other.MixPid_units, sizeof(*other.MixPid_units)*nj);
        memcpy(this->bemfToRaws, other.bemfToRaws, sizeof(*other.bemfToRaws)*nj);
        memcpy(this->ktauToRaws, other.ktauToRaws, sizeof(*other.ktauToRaws)*nj);
        memcpy(this->viscousPosToRaws, other.viscousPosToRaws, sizeof(*other.viscousPosToRaws)*nj);
        memcpy(this->viscousNegToRaws, other.viscousNegToRaws, sizeof(*other.viscousNegToRaws)*nj);
        memcpy(this->coulombPosToRaws, other.coulombPosToRaws, sizeof(*other.coulombPosToRaws)*nj);
        memcpy(this->coulombNegToRaws, other.coulombNegToRaws, sizeof(*other.coulombNegToRaws)*nj);
        memcpy(this->velocityThresToRaws, other.velocityThresToRaws, sizeof(*other.velocityThresToRaws)*nj);
    }
};

ControlBoardHelper::ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs, const double *newtons, const double *amps, const double *volts, const double *dutycycles, const double *kbemf, const double *ktau)
{
    yAssert(n>=0);         // if number of joints is negative complain!
    yAssert(aMap!=nullptr);      // at least the axisMap is required
    mPriv = new PrivateUnitsHandler(n);

    memcpy(mPriv->axisMap, aMap, sizeof(int)*n);

    if (zs != nullptr) {
        memcpy(mPriv->position_zeros, zs, sizeof(double) * n);
    }
    if (angToEncs != nullptr) {
        memcpy(mPriv->angleToEncoders, angToEncs, sizeof(double) * n);
    }
    if (newtons != nullptr) {
        memcpy(mPriv->newtonsToSensors, newtons, sizeof(double) * n);
    }
    if (amps != nullptr) {
        memcpy(mPriv->ampereToSensors, amps, sizeof(double) * n);
    }
    if (volts != nullptr) {
        memcpy(mPriv->voltToSensors, volts, sizeof(double) * n);
    }
    if (dutycycles != nullptr) {
        memcpy(mPriv->dutycycleToPWMs, dutycycles, sizeof(double) * n);
    }
    if (kbemf != nullptr) {
        memcpy(mPriv->bemfToRaws, kbemf, sizeof(double) * n);
    }
    if (ktau != nullptr) {
        memcpy(mPriv->ktauToRaws, ktau, sizeof(double) * n);
    }

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
    if (mPriv) { delete mPriv; mPriv = nullptr; }
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
    if( (id >= mPriv->nj) || (id< 0))
    {
        return false;
    }
    return true;
}


bool ControlBoardHelper::checkAxesIds(const int n_axes, const int* axesList)
{
    if(n_axes > mPriv->nj)
    {
        if (mPriv->verbose) {
            yError("checkAxesIds: num of axes is too big");
        }
        return false;
    }
    for(int idx = 0; idx<n_axes; idx++)
    {
        if( (axesList[idx]<0) || (axesList[idx]>= mPriv->nj) )
        {
            if (mPriv->verbose) {
                yError("checkAxesIds: joint id out of bound");
            }

            return false;
        }
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
    for (int k = 0; k < mPriv->nj; k++) {
        user[toUser(k)] = hwData[k];
    }
}

//map a vector, no conversion
void ControlBoardHelper::ControlBoardHelper::toUser(const int *hwData, int *user)
{
    for (int k = 0; k < mPriv->nj; k++) {
        user[toUser(k)] = hwData[k];
    }
}

//map a vector, no conversion
    void ControlBoardHelper::toHw(const double *usr, double *hwData)
{
    for (int k = 0; k < mPriv->nj; k++) {
        hwData[toHw(k)] = usr[k];
    }
}

//map a vector, no conversion
void ControlBoardHelper::toHw(const int *usr, int *hwData)
{
    for (int k = 0; k < mPriv->nj; k++) {
        hwData[toHw(k)] = usr[k];
    }
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
void ControlBoardHelper::bemf_user2raw(double bemf_user, int j, double &bemf_raw, int &k)
{
    bemf_raw = bemf_user * mPriv->bemfToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::ktau_user2raw(double ktau_user, int j, double &ktau_raw, int &k)
{
    ktau_raw = ktau_user * mPriv->ktauToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::viscousPos_user2raw(double viscousPos_user, int j, double &viscousPos_raw, int &k)
{
    viscousPos_raw = viscousPos_user * mPriv->viscousPosToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::viscousNeg_user2raw(double viscousNeg_user, int j, double &viscousNeg_raw, int &k)
{
    viscousNeg_raw = viscousNeg_user * mPriv->viscousNegToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::coulombPos_user2raw(double coulombPos_user, int j, double &coulombPos_raw, int &k)
{
    coulombPos_raw = coulombPos_user * mPriv->coulombPosToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::coulombNeg_user2raw(double coulombNeg_user, int j, double &coulombNeg_raw, int &k)
{
    coulombNeg_raw = coulombNeg_user * mPriv->coulombNegToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::velocityThres_user2raw(double velocityThres_user, int j, double &velocityThres_raw, int &k)
{
    velocityThres_raw = velocityThres_user * mPriv->velocityThresToRaws[j];
    k = toHw(j);
}

void ControlBoardHelper::bemf_raw2user(double bemf_raw, int k_raw, double &bemf_user, int &j_user)
{
    j_user = toUser(k_raw);
    bemf_user = bemf_raw / mPriv->bemfToRaws[j_user];
}

void ControlBoardHelper::ktau_raw2user(double ktau_raw, int k_raw, double &ktau_user, int &j_user)
{
    j_user = toUser(k_raw);
    ktau_user = ktau_raw / mPriv->ktauToRaws[j_user];
}

double  ControlBoardHelper::bemf_user2raw(double bemf_user, int j)
{
    return bemf_user * mPriv->bemfToRaws[j];
}

double  ControlBoardHelper::ktau_user2raw(double ktau_user, int j)
{
    return ktau_user * mPriv->ktauToRaws[j];
}

double  ControlBoardHelper::viscousPos_user2raw(double viscousPos_user, int j)
{
    return viscousPos_user * mPriv->viscousPosToRaws[j];
}

double ControlBoardHelper::viscousNeg_user2raw(double viscousNeg_user, int j)
{
    return viscousNeg_user * mPriv->viscousNegToRaws[j];
}

double ControlBoardHelper::coulombPos_user2raw(double coulombPos_user, int j)
{
    return coulombPos_user * mPriv->coulombPosToRaws[j];
}

double ControlBoardHelper::coulombNeg_user2raw(double coulombNeg_user, int j)
{
    return coulombNeg_user * mPriv->coulombNegToRaws[j];
}

double ControlBoardHelper::velocityThres_user2raw(double velocityThres_user, int j)
{
    return velocityThres_user * mPriv->velocityThresToRaws[j];
}

void ControlBoardHelper::viscousPos_raw2user(double viscousPos_raw, int k_raw, double &viscousPos_user, int &j_user)
{
    j_user = toUser(k_raw);
    viscousPos_user = viscousPos_raw / mPriv->viscousPosToRaws[j_user];
}

void ControlBoardHelper::viscousNeg_raw2user(double viscousNeg_raw, int k_raw, double &viscousNeg_user, int &j_user)
{
    j_user = toUser(k_raw);
    viscousNeg_user = viscousNeg_raw / mPriv->viscousNegToRaws[j_user];
}

void ControlBoardHelper::coulombPos_raw2user(double coulombPos_raw, int k_raw, double &coulombPos_user, int &j_user)
{
    j_user = toUser(k_raw);
    coulombPos_user = coulombPos_raw / mPriv->coulombPosToRaws[j_user];
}

void ControlBoardHelper::coulombNeg_raw2user(double coulombNeg_raw, int k_raw, double &coulombNeg_user, int &j_user)
{
    j_user = toUser(k_raw);
    coulombNeg_user = coulombNeg_raw / mPriv->coulombNegToRaws[j_user];
}

void ControlBoardHelper::velocityThres_raw2user(double velocityThres_raw, int k_raw, double &velocityThres_user, int &j_user)
{
    j_user = toUser(k_raw);
    velocityThres_user = velocityThres_raw / mPriv->velocityThresToRaws[j_user];
}


// *******************************************//

int ControlBoardHelper::axes()
{ return mPriv->nj; }

// *******************************************//

double ControlBoardHelper::get_pidoutput_conversion_factor_user2raw(const yarp::dev::PidControlTypeEnum& pidtype, int j)
{
    double output_conversion_factor;
    switch ((mPriv->pid_units[pidtype])[j].out_units)
    {
        case  PidOutputUnitsEnum::DUTYCYCLE_PWM_PERCENT:  output_conversion_factor = mPriv->dutycycleToPWMs[j]; break;
        case  PidOutputUnitsEnum::CURRENT_METRIC:  output_conversion_factor = mPriv->ampereToSensors[j]; break;
        case  PidOutputUnitsEnum::POSITION_METRIC:  output_conversion_factor = mPriv->angleToEncoders[j]; break;
        case  PidOutputUnitsEnum::VELOCITY_METRIC:  output_conversion_factor = mPriv->angleToEncoders[j]; break;
        case  PidOutputUnitsEnum::TORQUE_METRIC:  output_conversion_factor = mPriv->newtonsToSensors[j]; break;
        case  PidOutputUnitsEnum::RAW_MACHINE_UNITS:
        default: output_conversion_factor = mPriv->helper_ones[j]; break;
    }
    return output_conversion_factor;
}

double ControlBoardHelper::get_pidfeedback_conversion_factor_user2raw(const yarp::dev::PidControlTypeEnum& pidtype, int j)
{
    double feedback_conversion_factor = 0.0;
    switch (( mPriv->pid_units[pidtype])[j].fbk_units)
    {
        case PidFeedbackUnitsEnum::METRIC:
            switch (pidtype)
            {
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_DIRECT_3:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_MIXED_3:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_DIRECT_3:
                    feedback_conversion_factor = mPriv->angleToEncoders[j];
                    break;
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
                    feedback_conversion_factor = mPriv->newtonsToSensors[j];
                    break;
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
                case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
                    feedback_conversion_factor = mPriv->ampereToSensors[j];
                    break;
            }
        break;
        case PidFeedbackUnitsEnum::RAW_MACHINE_UNITS:
        default: feedback_conversion_factor = mPriv->helper_ones[j]; break;
    }
    return (1.0 / feedback_conversion_factor);
}
void ControlBoardHelper::convert_pid_to_user(const yarp::dev::PidControlTypeEnum& pidtype, const Pid &in_raw, int j_raw, Pid &out_usr, int &k_usr)
{
    ControlBoardHelper* cb_helper = this;
    k_usr = cb_helper->toUser(j_raw);
    out_usr = in_raw;

    //feedback is at the denominator, i.e. kp=[PWM/deg]
    double feedback_conversion_units_user2raw = get_pidfeedback_conversion_factor_user2raw(pidtype, k_usr);
    out_usr.kp = out_usr.kp / feedback_conversion_units_user2raw;
    out_usr.ki = out_usr.ki / feedback_conversion_units_user2raw;
    out_usr.kd = out_usr.kd / feedback_conversion_units_user2raw;

    //output is at the numerator, i.e. kp=[PWM/deg]
    double output_conversion_units_user2raw = get_pidoutput_conversion_factor_user2raw(pidtype, k_usr);
    out_usr.kp = out_usr.kp / output_conversion_units_user2raw;
    out_usr.ki = out_usr.ki / output_conversion_units_user2raw;
    out_usr.kd = out_usr.kd / output_conversion_units_user2raw;
    out_usr.max_output = out_usr.max_output / output_conversion_units_user2raw;
    out_usr.max_int = out_usr.max_int / output_conversion_units_user2raw;
    out_usr.stiction_up_val = out_usr.stiction_up_val / output_conversion_units_user2raw;
    out_usr.stiction_down_val = out_usr.stiction_down_val / output_conversion_units_user2raw;
    out_usr.offset = out_usr.offset / output_conversion_units_user2raw;
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
    k_raw = cb_helper->toHw(j_usr);
    out_raw = in_usr;

    //feedback is at the denominator, i.e. kp=[PWM/deg]
    double feedback_conversion_units_user2raw = get_pidfeedback_conversion_factor_user2raw(pidtype, j_usr);
    out_raw.kp = out_raw.kp * feedback_conversion_units_user2raw;
    out_raw.ki = out_raw.ki * feedback_conversion_units_user2raw;
    out_raw.kd = out_raw.kd * feedback_conversion_units_user2raw;

    //output is at the numerator, i.e. kp=[PWM/deg]
    double output_conversion_units_user2raw = get_pidoutput_conversion_factor_user2raw(pidtype, j_usr);
    out_raw.kp = out_raw.kp * output_conversion_units_user2raw;
    out_raw.ki = out_raw.ki * output_conversion_units_user2raw;
    out_raw.kd = out_raw.kd * output_conversion_units_user2raw;
    out_raw.max_output = out_raw.max_output * output_conversion_units_user2raw;
    out_raw.max_int = out_raw.max_int * output_conversion_units_user2raw;
    out_raw.stiction_up_val = out_raw.stiction_up_val * output_conversion_units_user2raw;
    out_raw.stiction_down_val = out_raw.stiction_down_val * output_conversion_units_user2raw;
    out_raw.offset = out_raw.offset * output_conversion_units_user2raw;
}

void ControlBoardHelper::convert_pidunits_to_machine(const yarp::dev::PidControlTypeEnum& pidtype, double userval, int j, double &machineval, int &k)
{
    ControlBoardHelper* cb_helper = this;
    switch (pidtype)
    {
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
        //Beware:posA2E employs zeros, not only angleToEncoders
        //This is fine if convert_pidunits_to_machine() is called by methods that are aware of this feature, that is intentional.
        cb_helper->posA2E(userval, j, machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
        cb_helper->velA2E(userval, j, machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
        cb_helper->trqN2S(userval, j, machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
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
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
        //Beware:posA2E employs zeros, not only angleToEncoders
        //This is fine if convert_pidunits_to_machine() is called by methods that are aware of this feature, that is intentional.
        cb_helper->posA2E(userval, machineval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
        cb_helper->velA2E(userval, machineval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
        cb_helper->trqN2S(userval, machineval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
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
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
        //Beware:posE2A employs zeros, not only angleToEncoders.
        //This is fine if convert_pidunits_to_user() is called by methods that are aware of this feature, that is intentional.
        *userval = cb_helper->posE2A(machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
        *userval = cb_helper->velE2A(machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
        *userval = cb_helper->trqS2N(machineval, k);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
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
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_POSITION_3:
        //Beware:posE2A employs zeros, not only angleToEncoders.
        //This is fine if convert_pidunits_to_user() is called by methods that are aware of this feature, that is intentional.
        cb_helper->posE2A(machineval, userval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_VELOCITY_3:
        cb_helper->velE2A(machineval, userval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_TORQUE_3:
        cb_helper->trqS2N(machineval, userval);
        break;
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_1:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_2:
    case yarp::dev::PidControlTypeEnum::VOCAB_PIDTYPE_CURRENT_3:
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
    for (int i = 0; i < nj; i++)
    {
        mPriv->pid_units[pidtype][i].fbk_units = fbk_conv_units;
        mPriv->pid_units[pidtype][i].out_units = out_conv_units;
    }
}
