featureMultipleAnalogSensorsRosPublishers {#master}
-----------------------------------------

### Devices

* Added the following new devices:

#### `IMURosPublisher`

* `IMURosPublisher` This wrapper connects to a device and publishes a ROS topic
  of type `sensor_msgs::Imu`.

#### `MagfieldRosPublisher`

* `MagfieldRosPublisher` This wrapper connects to a device and publishes a ROS
  topic of type `sensor_msgs::MagneticField`.

#### `PoseRosPublisher`

* `PoseRosPublisher` This wrapper connects to a device and publishes a ROS topic
  of type `geometry_msgs::PoseStamped`.

#### `TemperatureRosPublisher`

* `TemperatureRosPublisher` This wrapper connects to a device and publishes a
  ROS topic of type `sensor_msgs::Temperature`.

#### `WrenchRosPublisher`

* ``WrenchRosPublisher` This wrapper connects to a device and publishes a ROS
  topic of type `geometry_msgs::WrenchStamped`.
