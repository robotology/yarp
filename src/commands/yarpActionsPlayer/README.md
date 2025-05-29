\defgroup yarpactionsplayer yarpactionsplayer: an utility to replay robot movements

\ingroup yarp_commands

# yarpActionsPlayer

`yarpActionsPlayer` is an executable designed to control a robot (real or simulated) by sending timestamped position commands to its joints, enabling it to replicate specified trajectories.

## Startup
At the startup, the executable receives in input a set of files, each of them describing a trajectory for one or more joints. Each trajectory is called `action` and is identified by a name.
The trajectory files have the following format:
- The first column is a progressive counter (not used)
- The second column is a timestamp (in seconds)
- All subsequent columns are position values, one joint for each column.

Here is an example of a trajectory file for two joints, with positions sampled at a period of 10ms:
```
0    0.010   0.000   0.000
1    0.020   1.253   1.253
2    0.030   2.487   2.487
3    0.040   3.681   3.681
4    0.050   4.818   4.818
5    0.060   5.878   5.878
6    0.070   6.845   6.845
7    0.080   7.705   7.705
8    0.090   8.443   8.443
9    0.090   9.048   9.048
10   0.100   9.511   9.511
11   0.110   9.823   9.823
12   0.120   9.980   9.980
```
This format is consistent with the output of the state port of a `controlBoard_nws_yarp`.
The following command can be used to record the trajectory of a moving robot and obtain a trajectory file that can reproduced with `yarpActionsPlayer`.
```
yarp read ... /robot/part/state:o envelope > file.txt
```
The joints commands are assigned to the robot by using a `remoteControlBoardremapper` device. In this way, the user can select to work with joints also belonging to different parts of the robot and synchronize the movements between them.
The following file, configuration.ini (see the `example` folder) creates two different controllers, `controller1` and `controller2`. The first one will control the joints called `hjoint1` and `hjoint2` of the robot part `/robot/head`.
The second one will attach to two different parts of the robot, i.e. `/robot/head` `/robot/arm` and control the joints`hjoint1`,`ajoint1`,`ajoint3` which belong to these parts.
Please note that the number of joints described in the controller (and their order) must match the number of joints indicated in the trajectory file.
As shown in the following example, user has to associate a controller for each action file to correctly map the trajectories described in the file with the joints to actuate.
```
[CONTROLLERS]
controller1 (/robot/head) (hjoint1 hjoint2)
controller2 (/robot/head /robot/arm) (hjoint1 ajoint1 ajoint3)

[ACTIONS]
wave_hand        controller1   trajectory_file1.txt
rise_hand        controller1   trajectory_file2.txt
turn_head_left   controller2   trajectory_file3.txt
turn_head_right  controller2   trajectory_file4.txt
```
## Basic usage

After starting `yarpActionsPlayer` by providing the above mentioned configuration files, the module will wait to received user commands via rpc port, e.g. `/yarpActionsPlayer/rpc`
Use the `help` command to list all possible commands. The command `show_actions` will display the names of the actions loaded at startup.
The command `play <action_name>` will play an action once. The command `choose <action_name>` will allow to select an action to perform more advanced commands, such as changing its playback speed, pausing it during execution or setting it to loop continuously.

## Advanced considerations / Parameters tuning

`yarpActionsPlayer` uses a periodic thread to schedule the execution of the commanded position at the correct time. The option `--period` allows to set the period of this thread: small values allows to schedule the command with more accuracy. The suggested value is 0.005 seconds.

Another important parameter to consider is the trajectory sampling period. By default, the application does not internally resample loaded trajectories, allowing files with variable sampling rates to be used. However, users may choose to resample a trajectory to a specific rate using the `--resample` option.
When this option is enabled, all trajectories are resampled at the specified frequency using linear interpolation. This feature can help compensate for nonlinearities in the joint position controller’s response and reduce vibrations caused by overshoots (*)
It is generally recommended to keep this sampling period small, in the 0.005 - 0.0010 seconds range to avoid overshoots of the controller when performing a direct position control of the joint. Small values will also guarantee that the difference in position between two subsequent frames is small.

(*) The response of the controller for the following two set of trajectories is generally different, event if the final position is reached in the same amount of time:
```
q2(t2=0.1)  = 1
q1(t1=0.0)  = 0
```
```
q5(t5=0.1)   = 1
q4(t4=0.75)  = 0.75
q3(t3=0.5)   = 0.5
q2(t2=0.025) = 0.25
q1(t1=0.0)   = 0
```
In the first case, a large movement (1 degree) might cause overshoot, while in the second case, the phenomena is mitigated by keeping limited the reference delta to a small value.

## Joints initial position and robot safety

yarpActionsPlayer controls the robot using `positionDirect` control mode (see https://www.yarp.it/latest/classyarp_1_1dev_1_1IPositionDirect.html). In this mode, the target is provided as a step reference and the joints are commanded to reach the final position with no limitations in the the movement speed. This control mode is thus intrinsically unsafe, especially if the difference between the current and the commanded position is large. For this reason a number of safety checks are included in the control module.
- When the action is tarted, the robot will start to move in position mode (and not in position direct mode) to the first reference using a limited speed.
- Before starting the action playback, the module will check that the first reference is reached within a specified tolerance, which can be defined by the option `pos_tolerance` (default value: 2 degrees).
- If the option `pos_strict_check` is enabled (default value: false) the system will halt if the tolerance threshold is not reached, otherwise it will continue with normal operation afterwards, switching to position direct mode. The timeout is defined by the parameter `pos_timeout` (default value: 2seconds)
- By the default, launching the module will NOT control the robot, it will just simulate it. The user can enable the real control, after checking the correctness of the trajectories on a simulator, using the option `execute`.

## Examples

The `example` folder contains the `configuration.ini` and some test trajectories for a fake robot.
To try the the example, you must instantiate a fakerobot using the following commands:
```
yarpdev --device deviceBundler --wrapper_device controlBoard_nws_yarp --attached_device fakeMotionControl --name /robot/head   --GENERAL::Joints 3  --GENERAL::AxisName "(hjoint1 hjoint2 hjoint3)"
yarpdev --device deviceBundler --wrapper_device controlBoard_nws_yarp --attached_device fakeMotionControl --name /robot/arm    --GENERAL::Joints 6  --GENERAL::AxisName "(ajoint1 ajoint2 ajoint3 ajoint4 ajoint5 ajoint6)"
```
You can then observe the movement of the joints using `yarpmotorgui` or `yarpscope` executables.
