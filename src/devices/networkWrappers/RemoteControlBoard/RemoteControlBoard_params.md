* |            |  remote           | string  | -     |   -           | Yes          | Prefix of the port to which to connect.        |       |
* |            |  local            | string  | -     |   -           | Yes          | Port prefix of the port opened by this device. |       |
* |            |  writeStrict      | string  | -     |   -           | No           | It can be 'on' or 'off'                        | See implementation |
* |            |  carrier          | string  | -     |   udp         | No           | carrier used for receiving streamed robot state         |       |
* |            |  timeout          | float   | -     |   0.5         | No           | timeout for the input port which receives the streamed robot state |       |
* | local_qos  |  enable           | bool    | -     |   false       | No           | Enable the usage of local Qos |       |
* | local_qos  |  thread_priority  | int     | -     |   0           | No           | Local Qos. See https://yarp.it/latest/channelprioritization.html |       |
* | local_qos  |  thread_policy    | int     | -     |   0           | No           | Local Qos. See https://yarp.it/latest/channelprioritization.html |       |
* | local_qos  |  packet_priority  | string  | -     |               | No           | Local Qos. See https://yarp.it/latest/channelprioritization.html |       |
* | remote_qos |  enable           | bool    | -     |   false       | No           | Enable the usage of remote Qos |       |
* | remote_qos |  thread_priority  | int     | -     |   0           | No           | Remote Qos. See https://yarp.it/latest/channelprioritization.html |       |
* | remote_qos |  thread_policy    | int     | -     |   0           | No           | Remote Qos. See https://yarp.it/latest/channelprioritization.html. |       |
* | remote_qos |  packet_priority  | string  | -     |               | No           | Remote Qos. See https://yarp.it/latest/channelprioritization.html. |       |
* |            |  diagnostic       | bool    | -     |   false       | No           | For development purpose only |       |
