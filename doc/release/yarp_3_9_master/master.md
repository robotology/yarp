# Navigation2D nws

Added a status:o port to the navigation server.

# multipleanalogsensorsserver

Fixed bug that resulted in a segmentation fault if one of the device to which
`multipleanalogsensorsserver` was attached did not resized the measure vector.

# Tests
yarp tests moved from `tests` folder to individual library folders (e.g. libYARP_XXX)
No functional changes.

# audioRecoder_nws_yarp

Added new device `audioRecoder_nws_yarp` and thrift interface `IAudioGrabberMsgs`
Deprecated old device `AudioRecorderWrapper`.

# audioRecoder_nwc_yarp

Added new device `audioRecoder_nwc_yarp`.

# LLM_nws_yarp

Added new device `LLM_nws_yarp`, thrift interface `ILLMMsgs`, fakeDevice `fakeLLMDevice`.

# LLM_nwc_yarp

Added new device `LLM_nwc_yarp`.

# yarp::sig::Sound

`yarp::sig::sound` refactored to avoid the internal use (private implementation) of yarp::sig::Image data type.