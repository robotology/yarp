# Navigation2D nws

Added a status:o port to the navigation server.

# multipleanalogsensorsserver

Fixed bug that resulted in a segmentation fault if one of the device to which
`multipleanalogsensorsserver` was attached did not resized the measure vector.

# Tests
yarp tests moved from `tests` folder to individual library folders (e.g. libYARP_XXX)
No functional changes.