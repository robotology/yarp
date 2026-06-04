# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Loading yarp binding library
import yarp
import subprocess

multiplier = 2


def create():
    """
    create is called when the port monitor is created
    @return Boolean
    """

    return True


def destroy():
    """
    destroy is called when port monitor is destroyed
    """
    pass


def updateData(thing):
    """
    updateData is called when the port monitor receives new data
    to update before forwarding it.
    @param thing The Things abstract data type
    @return thing The updated Things object
    """

    global multiplier
    bottledThing = thing.asBottle()
    print(f"Received data: {bottledThing.toString()}")
    if bottledThing.size() == 2:
        first = bottledThing.get(0).asFloat64() * multiplier
        second = bottledThing.get(1).asFloat64()
        bottledThing.clear()
        bottledThing.addFloat64(first)
        bottledThing.addFloat64(second)
        # Send the current multiplier value to the clock port
        # admin_port.write(bottleOut)  # Send the multiplier value
        # multiplier *= 2  # Double the multiplier for the next update


    return thing
