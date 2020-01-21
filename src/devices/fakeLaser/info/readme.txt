Some examples:

* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test no_obstacles
All measurements obtained from laser device will be inf.

* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_pattern
The output should be similar to the one shown in these pictures:
ex1.jpg   x axis is pointing upward (N), y axis is pointg leftward (W)
ex2.jpg   laser data are increasing counterclockwise

* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map
A map is loaded. Laser data are generated assuming robot position is in 0,0,0

* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_port /fakeLaser/location:i
A map is loaded. Laser data are generated, with robot position obtained from a yarp port

* yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /ikart/laser:o --test use_mapfile --map_file mymap.map --localization_client /fakeLaser/localizationClient
A map is loaded. Laser data are generated, with robot position obtained from a localization client
