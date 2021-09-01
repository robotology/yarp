fix_audio_case {#yarp_3_5}
----------------------

## Libraries

### `dev`

* fixed the following git issue: `warning: the following paths have collided (e.g. case-sensitive paths
on a case-insensitive filesystem) and only one from the same` by removing stray files:
`src/libYARP_dev/src/idl_generated_code/yarp/dev/audioPlayerStatus_common.h` and
`src/libYARP_dev/src/idl_generated_code/yarp/dev/audioRecorderStatus_common.h`
