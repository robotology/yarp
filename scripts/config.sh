
# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

USE_SOURCEFORGE=false

if $USE_SOURCEFORGE; then
# leave WEB_USER blank to have it picked up from CVS
    WEB_USER=
    WEB_SERVER=yarp0.sourceforge.net
    WEB_DIR=/home/groups/y/ya/yarp0/htdocs
else
    WEB_USER=babybot
    WEB_SERVER=eris.humanoids.iit.it
    WEB_DIR=/var/www/html/yarp
fi

WEB_DOC_DIR=$WEB_DIR/specs/dox
WEB_DOWNLOAD_DIR=$WEB_DIR/downloads
