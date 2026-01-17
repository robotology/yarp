/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yPose6D {
} (
  yarp.name = "yarp::sig::Pose6D"
  yarp.includefile = "yarp/sig/Pose6D.h"
)

struct yColorRGB {
} (
  yarp.name = "yarp::sig::ColorRGB"
  yarp.includefile = "yarp/sig/ColorRGB.h"
)

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct getListReturnValue {
1: yReturnValue retVal;
2: list<string> listnames;
}

struct getPoseReturnValue {
1: yReturnValue retVal;
2: yPose6D pose;
}

//-----------------------------------------------------

service ISimulatedWorldMsgs
{
    yReturnValue makeSphere   (1: string id_name, 2: double radius,   3: yPose6D pose,     4: yColorRGB color,       5: string frame_name,    6: bool gravity_enable,   7: bool collision_enable);
    yReturnValue makeBox      (1: string id_name, 2: double width,    3: double height,    4: double thickness,      5: yPose6D pose,         6: yColorRGB color,       7: string frame_name,      8: bool gravity_enable,     9: bool collision_enable);
    yReturnValue makeCylinder (1: string id_name, 2: double radius,   3: double length,    4: yPose6D pose,          5: yColorRGB color,      6: string frame_name,     7: bool gravity_enable,    8: bool collision_enable);
    yReturnValue makeFrame    (1: string id_name, 2: double size,     3: yPose6D pose,     4: yColorRGB color,       5: string frame_name,    6: bool gravity_enable,   7: bool collision_enable);
    yReturnValue makeModel    (1: string id_name, 2: string filename, 3: yPose6D pose,     4: string frame_name,     5: bool gravity_enable,  6: bool collision_enable);

    yReturnValue changeColor (1: string id, 2: yColorRGB color);
    yReturnValue setPose(1: string id, 2: yPose6D pose, 3: string frame_name);
    yReturnValue enableGravity (1: string id, 2: bool enable);
    yReturnValue enableCollision (1: string id, 2: bool enable);
    getPoseReturnValue getPose(1:string id, 2: string frame_name);
    yReturnValue deleteObject(1:string id);
    yReturnValue deleteAll();
    getListReturnValue getList();
    yReturnValue attach(1:string id, 2:string link_name);
    yReturnValue detach(1:string id);
    yReturnValue rename(1:string old_name, 2:string new_name);
}