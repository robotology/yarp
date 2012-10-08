/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef ROBOTINTERFACE_ROBOTINTERFACE_H
#define ROBOTINTERFACE_ROBOTINTERFACE_H

namespace RobotInterface
{

class RobotInterface
{
public:
    explicit RobotInterface();
    virtual ~RobotInterface();

private:
    class Private;
    Private * const mPriv;
}; // class RobotInterface

} // namespace RobotInterface

#endif // ROBOTINTERFACE_ROBOTINTERFACE_H
