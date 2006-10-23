
USE_SOURCEFORGE=false

if $USE_SOURCEFORGE; then
# leave WEB_USER blank to have it picked up from CVS
    WEB_USER=
    WEB_SERVER=yarp0.sourceforge.net
    WEB_DIR=/home/groups/y/ya/yarp0/htdocs
else
    WEB_USER=$USER
    WEB_SERVER=eris.liralab.it
    WEB_DIR=/var/www/html/yarp
fi

WEB_DOC_DIR=$WEB_DIR/specs/dox
WEB_DOWNLOAD_DIR=$WEB_DIR/downloads
