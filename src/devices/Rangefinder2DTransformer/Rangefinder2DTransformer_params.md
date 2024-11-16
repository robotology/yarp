 * | Group    | Name                  | Type    | Units    | Default Value   | Required     | Description                                                          | Notes |
 * |:--------:|:---------------------:|:-------:|:--------:|:---------------:|:------------:|:--------------------------------------------------------------------:|:-----:|
 * |          | device_position_x     | double  | m        | 0               | No           | X coordinate of the virtual lidar                                    |       |
 * |          | device_position_y     | double  | m        | 0               | No           | Y coordinate of the virtual lidar                                    |       |
 * |          | device_position_theta | double  | deg      | 0               | No           | Theta coordinate of the virtual lidar                                |       |
 * |          | laser_frame_name      | string  | -        |                 | No           | If present, open a frameTranformClient to get the robot->laser transform  | If present, disables device_position_* parameters |
 * |          | robot_frame_name      | string  | -        |                 | No           | If present, open a frameTranformClient to get the robot->laser transform  | If present, disables device_position_* parameters      |
