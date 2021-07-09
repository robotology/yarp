audio_recorder_refactor {#master}
-------------------------

## Important Changes

### Devices

* `fakeMicrophone` now generates a fixed tone sound
* Added new device `audioFromFileDevice` wrapped by `audioRecorderWrapper`
* Added new device `audioToFileDevice` wrapped by `audioPlayerWrapper`
* `fakeMicrophone`, `audioFromFileDevice`, `PortAudioRecorderDeviceDriver` derive from base class `AudioRecorderDeviceDriver` 
* Added missing documentation to `audioRecorderWrapper`, `audioPlayerWrapper`

### libYARP_dev

* added new base class `AudioRecorderDeviceDriver`

### examples

* Removed deprecated example soundgrabber
