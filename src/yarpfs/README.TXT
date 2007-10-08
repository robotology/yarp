
This directory is NOT included in the main YARP build.

It contains experimental code for creating a mapping from the
YARP Network to a file system, using the super-cool FUSE library.

The code is just a start, hardly anything is implemented yet.
Stay away if you wish to preserve your sanity.

  --paulfitz

===================================================================

If you want to play with this here are some tips:

+ Make sure you are using YARP from CVS.  The name server was updated
  to simplify yarpfs.

+ Learn about sshfs and get it installed.  This hasn't anything to
  do with yarpfs, it will just force you through the first
  set of hurdles that aren't yet documented for yarpfs (installing
  fuse kernel module if needed, learning how to mount things, etc).
  If you can't get sshfs working, you won't be able to get yarpfs
  working either.

+ Install the development environment for fuse (on debian: libfuse-dev)

+ Run cmake and compile

+ Make a directory for mounting, e.g.: mount /mnt/yarp

+ Run server in debug mode, e.g.: sudo ./yarpfs /mnt/yarp -d

+ Change into the directory and look around: cd /mnt/yarp

+ You should see a directory tree reflecting the ports that
  exist in your YARP Network.  

+ The "leaf" files called "read" and "write" can be read from
  and written to, and the data comes from and goes to the port
  associated with their containing directory.