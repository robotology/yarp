
#PLATFORMS="etch lenny squeeze wheezy lucid natty oneiric precise quantal "
PLATFORMS="quantal"

HARDWARE="i386 amd64"

# exceptions can be added as follows
# SKIP_lenny_amd64=1
SKIP_etch_i386=1
SKIP_etch_amd64=1

etch_MIRROR="http://archive.debian.org/debian/"
lenny_MIRROR="http://archive.debian.org/debian/"
squeeze_MIRROR="http://mirror3.mirror.garr.it/mirrors/debian/"

TESTING=
YARP_REVISION=9025
