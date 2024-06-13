* | | comport        | string |       |                            | No | full name of device file     | ex '/dev/ttyUSB0', it is mutually exclusive with 'i2c' parameter|
* | | baudrate       | int    |       | 115200                     | No | baudrate setting of COM port | used only with serial configuration |
* | | i2c            | string |       |                            | No | full name of device file     | ex '/dev/i2c-5', it is mutually exclusive with 'comport' parameter, necessary for i2c configuration|
* | | period         | int    | ms    |       10                   | No | period of the thread         | |
* | | sensor_name    | string |       | sensor_imu_bosch_bno055    | No | full name of the device      | |
* | | frame_name     | string |       | sensor_imu_bosch_bno055    | No | full name of the sensor frame in which the measurements are expressed | |
* | | channels       | int    |       |       12                   | No | number of channels in the output port. Default 12. If 16, also includes quaternion data | |
