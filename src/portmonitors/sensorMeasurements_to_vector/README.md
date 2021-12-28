
sensorMeasurements_to_vector plugin
======================================================================
Portmonitor plugin for converting a sensorMeasurements to yarp::sig::Vector
Note: This plugin is experimental and could be modified without any warning.
Note: This plugin should be considered an example. User are encouraged to take inspiration from it and adapt it to their needs.
      Only OrientationSensors, PositionSensors data types are supported. The plugin can be extended to support other data types.   Some restriction are apply on the size of the input data type vector. The output is a vector with a fixed size of six elements.
      The plugin can be extended to support vectors of different sizes.

Usage:
-----

yarpdev --device fakePositionSensor --name /tracking --period 10
yarp read ... /in
yarp connect yarp connect /tracking/measures:o /in tcp+recv.portmonitor+type.dll+file.sensorMeasurements_to_vector
