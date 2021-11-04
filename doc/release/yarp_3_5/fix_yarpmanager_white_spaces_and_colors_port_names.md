fix_yarpmanager_white_spaces_and_colors_port_names {#yarp_3_5}
-------------------

### Tools + Libraries

#### `yarpmanager` + `manager`

* Targeted issue: port names containing trailing or leading white spaces not targeted as `non-existing` but throwing an error
when trying to connect.
* Now `yarpmanager` shows an error message in its `Message` text box when a port name listed in the `Connection list` widget
contains white spaces. For simplicity's sake the error is thrown for spaces in any position, not only leading or trailing
* This has been done by adding a check in the [Manager::existPortFrom](https://github.com/robotology/yarp/blob/2aeab6c8e4babe651febc2ea755a5409f4cfeda7/src/libYARP_manager/src/yarp/manager/manager.cpp#L731) and [Manager::existPortTo](https://github.com/robotology/yarp/blob/2aeab6c8e4babe651febc2ea755a5409f4cfeda7/src/libYARP_manager/src/yarp/manager/manager.cpp#L745) functions
* Also, fixed the color of port names in the `Connections list` widget. If a port name is edited by the user, the new name
is automatically checked and if the port exists, the color is changed to green, if it doesn't the color is changed to red.
