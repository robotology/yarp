# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Loading yarp binding library
import yarp
import subprocess

# Global variables for QOS monitoring
admin_port = None
counter = 0
divider = 5


def create():
    """
    create is called when the port monitor is created
    @return Boolean
    """
    admin_port = yarp.Port()
    admin_port.setAdminMode(True)
    ret = admin_port.open("...")
    ret = ret and yarp.NetworkBase_connect(admin_port.getName(), "/clock")

    if ret:
        print("QOS monitor started!")
        return True
    else:
        print("Failed to start QOS monitor!")
        return False


def destroy():
    """
    destroy is called when port monitor is destroyed
    """
    global admin_port

    if admin_port is not None:
        yarp.NetworkBase_disconnect(admin_port.getName(), "/clock")
        admin_port.close()
    print("QOS monitor finished!")


def updateData(thing):
    """
    accept is called when the port receives new data
    @param thing The Things abstract data type
    @return Boolean
    if False is returned, the data will be ignored
    and update() will never be called
    """

    global counter, divider

    print("QOS monitor accepted data")
    counter += 1
    if counter % divider == 0:
        subprocess.run([
            "aplay",
            "/usr/share/sounds/freedesktop/stereo/camera-shutter.oga"
        ])
        counter = 0

    return True
