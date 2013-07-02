// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#ifndef IVELOCITYCONTROLIMPL_V2_H_
#define IVELOCITYCONTROLIMPL_V2_H_


#include <yarp/dev/IVelocityControl2.h>

namespace yarp {
  namespace dev {
    class ImplementVelocityControl2;
//    class IVelocityControl2Raw;
  }
}

class yarp::dev::ImplementVelocityControl2 : public IVelocityControl2
{
  protected:
    IVelocityControl2Raw *iVelocity2;
    void    *helper;
    int     *temp_int;
    double  *temp_double;
    Pid     *tempPids;

    /**
     * Initialize the internal data and alloc memory.
     * @param size is the number of controlled axes the driver deals with.
     * @param amap is a lookup table mapping axes onto physical drivers.
     * @param enc is an array containing the encoder to angles conversion factors.
     * @param zos is an array containing the zeros of the encoders.
     *  respect to the control/output values of the driver.
     * @return true if initialized succeeded, false if it wasn't executed, or assert.
     */
    bool initialize (int size, const int *amap, const double *enc, const double *zos);

    /**
     * Clean up internal data and memory.
     * @return true if uninitialization is executed, false otherwise.
     */
    bool uninitialize();

  public:
    /**
     * Constructor.
     * @param y is the pointer to the class instance inheriting from this
     *  implementation.
     */
    ImplementVelocityControl2(yarp::dev::IVelocityControl2Raw *y);

    /**
     * Destructor.
     */
    virtual ~ImplementVelocityControl2();

    virtual bool getAxes(int *axes);
    virtual bool setVelocityMode();
    virtual bool velocityMove(int j, double sp);
    virtual bool velocityMove(const double *sp);
    virtual bool setRefAcceleration(int j, double acc);
    virtual bool setRefAccelerations(const double *accs);
    virtual bool getRefAcceleration(int j, double *acc);
    virtual bool getRefAccelerations(double *accs);
    virtual bool stop(int j);
    virtual bool stop();


    // specific of IVelocityControl2
    virtual bool velocityMove(const int n_joint, const int *joints, const double *spds);
    virtual bool setRefAccelerations(const int n_joint, const int *joints, const double *accs);
    virtual bool getRefAccelerations(const int n_joint, const int *joints, double *accs);
    virtual bool stop(const int n_joint, const int *joints);
    virtual bool setVelPid(int j, const Pid &pid);
    virtual bool setVelPids(const Pid *pids);
    virtual bool getVelPid(int j, Pid *pid);
    virtual bool getVelPids(Pid *pids);
};

#endif /* IVELOCITYCONTROLIMPL_V2_H_ */
