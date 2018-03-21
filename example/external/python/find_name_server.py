#!/usr/bin/python

# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

import os

def find_name_server():
    # ==================================================================
    # WARNING: This code is deprecated and will no longer work,
    #          see ResourceFinder documentation for paths searched
    #          by YARP
    if "YARP_CONF" in os.environ.keys():
        base = os.environ["YARP_CONF"]
    elif "HOMEDIR" in os.environ.keys():
        base = os.path.join(os.environ["HOMEDIR"],"yarp","conf")
    elif "HOME" in os.environ.keys():
        base = os.path.join(os.environ["HOME"],".yarp","conf")
    else:
        print "Please set YARP_CONF to the location reported by this command:"
        print "  yarp conf"
        return None
    # ==================================================================

    print "Config files should be in", base

    namespace = "/root"
    server_filename = "yarp.conf"
    namespace_filename = os.path.join(base,"yarp_namespace.conf")
    if os.path.exists(namespace_filename):
        namespace = open(namespace_filename).read().strip()
        namespace = namespace.replace("\"","")
        print "Namespace set to", namespace
        server_filename = namespace.replace("/","_") + ".conf"
    else:
        print "Using default namespace,", namespace
    print "Expect name server information in", server_filename
    server_filename = os.path.join(base,server_filename)
    if os.path.exists(server_filename):
        info = open(server_filename).readline().strip().split()
        info = (info[0], int(info[1]))
        print "server contact information:", info
    else:
        print "cannot find", server_filename
    return info
