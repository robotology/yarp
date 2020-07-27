featureLaserFromPointCloudAndMore {#master}
---------------------------------

### Devices

#### `fakeLaser`

* added option --use_constant

#### `transformServer`

* cleanup of user option --USER_TF

#### `transformClient`

* added support for identical frame transforms.

#### `LaserFromPointCloud`

* Several improvements to LaserFromPointCloud device. 
* LaserFromPointCloud is a new device which exposes depth data received by a RGBD client through the standard interface IRangefinder2D.

#### `LaserFromExternalPort`

* Several improvements to LaserFromExternalPort device. 
* LaserFromExternalPort is a new device which exposes laser data received by a port through the standard interface IRangefinder2D.

### GUIs

#### `yarplaserscannergui`
* now multiple instances are allowed with option --local
* added option --debug


