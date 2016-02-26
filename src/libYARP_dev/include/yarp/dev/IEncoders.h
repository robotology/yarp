/*
 * Copyright (C) 2012 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARPIENCODERS
#define YARPIENCODERS

#include <yarp/os/Vocab.h>

namespace yarp {
    namespace dev {
        class IEncodersRaw;
        class IEncoders;
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class yarp::dev::IEncodersRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IEncodersRaw() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax) = 0;

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j encoder number
     * @return true/false
     */
    virtual bool resetEncoderRaw(int j)=0;

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    virtual bool resetEncodersRaw()=0;

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    virtual bool setEncoderRaw(int j, double val)=0;

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setEncodersRaw(const double *vals)=0;

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoderRaw(int j, double *v)=0;

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getEncodersRaw(double *encs)=0;

    /**
     * Read the instantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeedRaw(int j, double *sp)=0;

    /**
     * Read the instantaneous acceleration of an axis.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeedsRaw(double *spds)=0;

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    virtual bool getEncoderAccelerationRaw(int j, double *spds)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    virtual bool getEncoderAccelerationsRaw(double *accs)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class YARP_dev_API yarp::dev::IEncoders
{
public:
    /**
     * Destructor.
     */
    virtual ~IEncoders() {}

    /**
     * Get the number of controlled axes. This command asks the number of controlled
     * axes for the current physical interface.
     * @return the number of controlled axes.
     */
    virtual bool getAxes(int *ax) = 0;

    /**
     * Reset encoder, single joint. Set the encoder value to zero
     * @param j encoder number
     * @return true/false
     */
    virtual bool resetEncoder(int j)=0;

    /**
     * Reset encoders. Set the encoders value to zero
     * @return true/false
     */
    virtual bool resetEncoders()=0;

    /**
     * Set the value of the encoder for a given joint.
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    virtual bool setEncoder(int j, double val)=0;

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setEncoders(const double *vals)=0;

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getEncoder(int j, double *v)=0;

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getEncoders(double *encs)=0;

    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getEncoderSpeed(int j, double *sp)=0;

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getEncoderSpeeds(double *spds)=0;

    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    virtual bool getEncoderAcceleration(int j, double *spds)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens.
     */
    virtual bool getEncoderAccelerations(double *accs)=0;
};

// interface IEncoders sets
#define VOCAB_E_RESET VOCAB3('e','r','e')
#define VOCAB_E_RESETS VOCAB4('e','r','e','s')
#define VOCAB_ENCODER VOCAB3('e','n','c')
#define VOCAB_ENCODERS VOCAB4('e','n','c','s')

// interface IEncoders gets
#define VOCAB_ENCODER_SPEED VOCAB3('e','s','p')
#define VOCAB_ENCODER_SPEEDS VOCAB4('e','s','p','s')
#define VOCAB_ENCODER_ACCELERATION VOCAB3('e','a','c')
#define VOCAB_ENCODER_ACCELERATIONS VOCAB4('e','a','c','s')

#endif
