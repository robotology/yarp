#!/bin/bash
#export BUNDLE_YARP_VERSION=2.3.20
export BUNDLE_YARP_REVISION=5286
export BUNDLE_ACE_VERSION=6.1.0
#export BUNDLE_ACE_VERSION=6.0.1
export BUNDLE_CMAKE_VERSION=2.8.9
export BUNDLE_NSIS_VERSION=2.46
export BUNDLE_GSL_VERSION=1.14
export BUNDLE_GTKMM_VERSION=2.22.0-2
export BUNDLE_TWEAK=3

# override GTKMM package with Lorenzo's ZIP file
export BUNDLE_GTKMM_32_ZIP=http://eris.liralab.it/iCub/downloads/packages/windows/common/gtkmm-2.22-0-2.zip
export BUNDLE_GTKMM_64_ZIP=http://eris.liralab.it/iCub/downloads/packages/windows/common/gtkmm-2.22-win64.zip
