YARP <yarp-3.12> (UNRELEASED)                                         {#yarp_3_12}
============================

[TOC]

YARP <yarp-3.12> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.12%22).


## libYARP_dev

* Fixed linker error in `yarp::dev:: IJacobianCoupling interface

* Added new methods to the interface `yarp::dev::Nav2D::IMap2D`
   yarp::dev::ReturnValue reloadMapsCollection() override;
   yarp::dev::ReturnValue reloadLocationsAndExtras() override;
  Implemented in devices: `Map2D_nwc_yarp`, `Navigation2D_nwc_yarp`

* Added new method to the interface `yarp::dev::Nav2D::INavigation2DExtraActions`
   yarp::dev::ReturnValue inWhichAreaIAm(std::string& area_name, Nav2D::Map2DArea& area)`
  Implemented in devices: `Navigation2D_nwc_yarp`

* Added new datatype: `yarp::dev::Nav2D::Map2DObject`

* Added new methods to the  interface `yarp::dev::Nav2D::IMap2D`
    yarp::dev::ReturnValue storeObject(std::string object_name, yarp::dev::Nav2D::Map2DObject obj) override;
    yarp::dev::ReturnValue getObject(std::string object_name, yarp::dev::Nav2D::Map2DObject& obj) override;
    yarp::dev::ReturnValue getObjectsList(std::vector<std::string> & locations) override;
    yarp::dev::ReturnValue getAllObjects(std::vector<yarp::dev::Nav2D::Map2DObject>& obj) override;
    yarp::dev::ReturnValue renameObject(std::string original_name, std::string new_name) override;
    yarp::dev::ReturnValue deleteObject(std::string object_name) override;
    yarp::dev::ReturnValue clearAllObjects() override;
  Implemented in devices: `Map2DStorage`, `Map2D_nwc_yarp`, `Navigation2D_nwc_yarp`

## Devices

### MultipleAnalogSensorsRemapper

* Fixed bug in ReturnValue

## Tests

* Improved tests for device `Map2DStorage`, `ControlBoardRemapper`, `ILLM` interface

## Commands

### `yarpActionsPlayer`

* Improvements

## Bindings

* Improved yarp.i
* Added bindings for `yarp::dev::IBattery` interface

## Other

* Improved Bash completion script