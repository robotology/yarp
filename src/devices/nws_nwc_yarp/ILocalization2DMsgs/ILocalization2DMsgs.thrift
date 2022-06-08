/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yarp_sig_Matrix {
} (
  yarp.name = "yarp::sig::Matrix"
  yarp.includefile="yarp/sig/Matrix.h"
)

struct yarp_dev_Nav2D_Map2DLocation {
} (
  yarp.name = "yarp::dev::Nav2D::Map2DLocation"
  yarp.includefile = "yarp/dev/Map2DLocation.h"
)

struct yarp_dev_OdometryData {
} (
  yarp.name = "yarp::dev::OdometryData"
  yarp.includefile="yarp/dev/OdometryData.h"
)

enum yarp_dev_Nav2D_LocalizationStatusEnum {
} (
  yarp.name = "yarp::dev::Nav2D::LocalizationStatusEnum"
  yarp.includefile = "yarp/dev/ILocalization2D.h"
  yarp.enumbase = "yarp::conf::vocab32_t"
)

struct return_get_localization_status {
  1: bool ret = false;
  2: yarp_dev_Nav2D_LocalizationStatusEnum status ( yarp.vocab = "true" );
}

struct return_get_estimated_poses {
  1: bool ret = false;
  2: list<yarp_dev_Nav2D_Map2DLocation> poses;
}

struct return_get_current_position1 {
  1: bool ret = false;
  2: yarp_dev_Nav2D_Map2DLocation loc;
}

struct return_get_current_position2 {
  1: bool ret = false;
  2: yarp_dev_Nav2D_Map2DLocation loc;
  3: yarp_sig_Matrix cov;
}

struct return_get_estimated_odometry {
  1: bool ret = false;
  2: yarp_dev_OdometryData odom;
}

service ILocalization2DMsgs
{
  bool                               start_localization_service_RPC ();
  bool                               stop_localization_service_RPC  ();
  return_get_localization_status     get_localization_status_RPC    ();
  return_get_estimated_poses         get_estimated_poses_RPC        ();
  return_get_current_position1       get_current_position1_RPC      ();
  return_get_current_position2       get_current_position2_RPC      ();
  return_get_estimated_odometry      get_estimated_odometry_RPC     ();
  bool                               set_initial_pose1_RPC          (1: yarp_dev_Nav2D_Map2DLocation loc);
  bool                               set_initial_pose2_RPC          (1: yarp_dev_Nav2D_Map2DLocation loc, 2: yarp_sig_Matrix cov);
}
