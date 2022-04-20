
sound_marker_portmonitor plugin
======================================================================
This portmonitor adds a marker (i.e. extra audio samples with a specific pattern) at the end of each processed yarp::sig::Sound

Usage:
-----
yarpdev --device AudioRecorderWrapper --subdevice portaudioRecorder --start --min_samples_over_network 3200 --max_samples_over_network 3200 --AUDIO_BASE::rate 16000 --AUDIO_BASE::samples 6400 --AUDIO_BASE::channels 1
yarpdev --device AudioPlayerWrapper --subdevice audioToFileDevice --start
yarp connect /audioRecorderWrapper/audio:o /audioPlayerWrapper/audio:i tcp+recv.portmonitor+file.sound_marker+type.dll
