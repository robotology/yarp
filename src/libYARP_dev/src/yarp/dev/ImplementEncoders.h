/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_DEV_IMPLEMENTENCODERS_H
#define YARP_DEV_IMPLEMENTENCODERS_H

#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/api.h>

namespace yarp
{
    namespace dev
    {
        class ImplementEncoders;
    }
}

class YARP_dev_API yarp::dev::ImplementEncoders : public IEncoders
{
protected:
    IEncodersRaw *iEncoders;
    void *helper;
    double *temp;

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
    ImplementEncoders(yarp::dev::IEncodersRaw  *y);

    /**
     * Destructor. Perform uninitialize if needed.
     */
    virtual ~ImplementEncoders();


    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    bool getAxes(int *ax) override;

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j encoder number
     * @return true/false
     */
    bool resetEncoder(int j) override;

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    bool resetEncoders() override;

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    bool setEncoder(int j, double val) override;

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    bool setEncoders(const double *vals) override;

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    bool getEncoder(int j, double *v) override;

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    bool getEncoders(double *encs) override;

    /**
     * Read the instantaneous speed of an axis.
     * @param j axis number
     * @param spds pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    bool getEncoderSpeed(int j, double *spds) override;

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    bool getEncoderSpeeds(double *spds) override;

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    bool getEncoderAcceleration(int j, double *spds) override;

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    bool getEncoderAccelerations(double *accs) override;
};

#endif // YARP_DEV_IMPLEMENTENCODERS_H
