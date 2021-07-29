
soundfilter_resample_portmonitor plugin
======================================================================
Portmonitor plugin for extracting a channel from a sound stream and resample it to a specific bitrate

Usage:
-----

yarp connect /audioRecorderWrapper/audio:o /audioPlayerWrapper/audio:i tcp+recv.portmonitor+file.soundfilter_resample+type.dll+channel.0+frequency.16000+gain_percent.200
