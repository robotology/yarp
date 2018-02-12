/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Marco Randazzo <marco.randazzo@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_DEV_IMPLEMENTMOTORENCODERS_H
#define YARP_DEV_IMPLEMENTMOTORENCODERS_H

#include <yarp/dev/IMotorEncoders.h>

namespace yarp {
    namespace dev {
        class ImplementMotorEncoders;
    }
}

class YARP_dev_API yarp::dev::ImplementMotorEncoders: public IMotorEncoders
{
protected:
    IMotorEncodersRaw *iMotorEncoders;
    void *helper;
    double *temp;
    double *temp2;


    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize ();

public:
    /* Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementMotorEncoders(yarp::dev::IMotorEncodersRaw *y);


    virtual ~ImplementMotorEncoders();

    virtual bool getNumberOfMotorEncoders(int *num) override;

    virtual bool resetMotorEncoder(int m) override;
    virtual bool resetMotorEncoders() override;
    virtual bool setMotorEncoder(int m, const double val) override;
    virtual bool setMotorEncoders(const double *vals) override;
    virtual bool setMotorEncoderCountsPerRevolution(int m, const double cpr) override;
    virtual bool getMotorEncoderCountsPerRevolution(int m, double *cpr) override;
    virtual bool getMotorEncoder(int m, double *v) override;
    virtual bool getMotorEncodersTimed(double *encs, double *time) override;
    virtual bool getMotorEncoderTimed(int m, double *v, double *t) override;
    virtual bool getMotorEncoders(double *encs) override;
    virtual bool getMotorEncoderSpeed(int m, double *spds) override;
    virtual bool getMotorEncoderSpeeds(double *spds) override;
    virtual bool getMotorEncoderAcceleration(int m, double *spds) override;
    virtual bool getMotorEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTMOTORENCODERS_H
