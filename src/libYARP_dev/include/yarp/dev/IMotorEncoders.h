// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2012 Robotics Brain and Cognitive Sciences Department, Istituto Italiano di Tecnologia
 * Authors: Lorenzo Natale
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef __YARPIMOTORENCODERS__
#define __YARPIMOTORENCODERS__

#include <yarp/os/Vocab.h>

namespace yarp {
    namespace dev {
        class IMotorEncodersRaw;
        class IMotorEncoders;
      }
}

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class yarp::dev::IMotorEncodersRaw
{
public:
    /**
     * Destructor.
     */
    virtual ~IMotorEncodersRaw() {}

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
    virtual bool resetMotorEncoderRaw(int j)=0;

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
    virtual bool resetMotorEncodersRaw()=0;

    /**
     * Set the value of the encoder for a given joint. 
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    virtual bool setMotorEncoderRaw(int j, double val)=0;

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setMotorEncodersRaw(const double *vals)=0;

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getMotorEncoderRaw(int j, double *v)=0;

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getMotorEncodersRaw(double *encs)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * \param encs pointer to the array that will contain the output
     * \param stamps pointer to the array that will contain individual timestamps
     * \return return true if all goes well, false if anything bad happens.
     */
    virtual bool getMotorEncodersTimedRaw(double *encs, double *stamps)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * \param j axis index
     * \param encs encoder value
     * \param stamp corresponding timestamp
     * \return true if all goes well, false if anything bad happens.
     */
    virtual bool getMotorEncoderTimedRaw(int j, double *encs, double *stamp)=0;

    /**
     * Read the instantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getMotorEncoderSpeedRaw(int j, double *sp)=0;

    /**
     * Read the instantaneous acceleration of an axis.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getMotorEncoderSpeedsRaw(double *spds)=0;
    
    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    virtual bool getMotorEncoderAccelerationRaw(int j, double *spds)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens. 
     */
    virtual bool getMotorEncoderAccelerationsRaw(double *accs)=0;
};

/**
 * @ingroup dev_iface_motor
 *
 * Control board, encoder interface.
 */
class YARP_dev_API yarp::dev::IMotorEncoders
{
public:
    /**
     * Destructor.
     */
    virtual ~IMotorEncoders() {}

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
    virtual bool resetMotorEncoder(int j)=0;

    /**
     * Reset encoders. Set the encoders value to zero 
     * @return true/false
     */
    virtual bool resetMotorEncoders()=0;

    /**
     * Set the value of the encoder for a given joint. 
     * @param j encoder number
     * @param val new value
     * @return true/false
     */
    virtual bool setMotorEncoder(int j, double val)=0;

    /**
     * Set the value of all encoders.
     * @param vals pointer to the new values
     * @return true/false
     */
    virtual bool setMotorEncoders(const double *vals)=0;

    /**
     * Read the value of an encoder.
     * @param j encoder number
     * @param v pointer to storage for the return value
     * @return true/false, upon success/failure (you knew it, uh?)
     */
    virtual bool getMotorEncoder(int j, double *v)=0;

    /**
     * Read the position of all axes.
     * @param encs pointer to the array that will contain the output
     * @return true/false on success/failure
     */
    virtual bool getMotorEncoders(double *encs)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * \param encs pointer to the array that will contain the output
     * \param time pointer to the array that will contain individual timestamps
     * \return true if all goes well, false if anything bad happens.
     */
    virtual bool getMotorEncodersTimed(double *encs, double *time)=0;

   /**
    * Read the instantaneous acceleration of all axes.
    * \param j axis index
    * \param encs encoder value (pointer to)
    * \param time corresponding timestamp (pointer to)
    * \return true if all goes well, false if anything bad happens.
    */
    virtual bool getMotorEncoderTimed(int j, double *encs, double *time)=0;

    /**
     * Read the istantaneous speed of an axis.
     * @param j axis number
     * @param sp pointer to storage for the output
     * @return true if successful, false ... otherwise.
     */
    virtual bool getMotorEncoderSpeed(int j, double *sp)=0;

    /**
     * Read the instantaneous speed of all axes.
     * @param spds pointer to storage for the output values
     * @return guess what? (true/false on success or failure).
     */
    virtual bool getMotorEncoderSpeeds(double *spds)=0;
    
    /**
     * Read the instantaneous acceleration of an axis.
     * @param j axis number
     * @param spds pointer to the array that will contain the output
     */
    virtual bool getMotorEncoderAcceleration(int j, double *spds)=0;

    /**
     * Read the instantaneous acceleration of all axes.
     * @param accs pointer to the array that will contain the output
     * @return true if all goes well, false if anything bad happens. 
     */
    virtual bool getMotorEncoderAccelerations(double *accs)=0;
};

// interface IMotorEncoders sets
#define VOCAB_MOTOR_E_RESET VOCAB3('m','r','e')
#define VOCAB_MOTOR_E_RESETS VOCAB4('m','r','e','s')
#define VOCAB_MOTOR_ENCODER VOCAB3('m','n','c')
#define VOCAB_MOTOR_ENCODERS VOCAB4('m','n','c','s')

// interface IMotorEncoders gets
#define VOCAB_MOTOR_ENCODER_SPEED VOCAB3('m','s','p')
#define VOCAB_MOTOR_ENCODER_SPEEDS VOCAB4('m','s','p','s')
#define VOCAB_MOTOR_ENCODER_ACCELERATION VOCAB3('m','a','c')
#define VOCAB_MOTOR_ENCODER_ACCELERATIONS VOCAB4('m','a','c','s')

#endif


