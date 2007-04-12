This is an example to show how you can add external modules 
to your yarp repository.

Here you find a directory structure with two modules 
(foo and baz) which contains a set of devices (fooBot,
fooDevice, bazBot, bazDevice).

Each module will become a separate library, cmake will add code
to include them to the yarp factory of devices.

What you need to do:
- Create or edit $YARP_ROOT/conf/ExternalModules.cmake write the 
following lines:

SET(EXTERNAL_MODULES bazModule fooModule)
SET(bazModule_PATH "YARP_ROOT/example/externalmodules/baz")
SET(fooModule_PATH "YARP_ROOT/example/externalmodules/foo")

Notice that YARP_ROOT here should be substituted with the 
absolute path to the root of your YARP installation. This value
tells cmake/yarp the location of each module.

- Run cmake in $YARP_ROOT

New entries should now appear in the list of the available 
devices. The new devices should be bazDevice, bazBot, fooDevice 
and fooBot. Turn them on and finish the cmake process to generate
your project/make files.

- Build yarp.

- Run yarpdev --list, the new devices should appear in the list
(fooDevice, fooBot, bazDevice and bazBot).
