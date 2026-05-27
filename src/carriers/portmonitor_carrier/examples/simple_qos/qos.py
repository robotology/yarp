# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Loading yarp binding library
import yarp

# Global variables for QOS monitoring
prev_rcv_time = None
prev_monitor_time = None
frame_jitter = 0
frame_count = 0
admin_port = None


def create():
    """
    create is called when the port monitor is created
    @return Boolean
    """
    global prev_rcv_time, prev_monitor_time, frame_jitter, frame_count, admin_port

    prev_rcv_time = None
    prev_monitor_time = None
    frame_jitter = 0
    frame_count = 0

    admin_port = yarp.Port()
    admin_port.setAdminMode(True)
    ret = admin_port.open("...")
    ret = ret and yarp.NetworkBase_connect(admin_port.getName(), "/grabber")

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
        yarp.NetworkBase_disconnect(admin_port.getName(), "/grabber")
        admin_port.close()
    print("QOS monitor finished!")


def accept(thing):
    """
    accept is called when the port receives new data
    @param thing The Things abstract data type
    @return Boolean
    if False is returned, the data will be ignored
    and update() will never be called
    """
    global prev_rcv_time, prev_monitor_time, frame_jitter, frame_count, admin_port

    if prev_rcv_time is None:
        prev_rcv_time = yarp.Time_now()
        prev_monitor_time = yarp.Time_now()
        return True

    frame_jitter = frame_jitter + (yarp.Time_now() - prev_rcv_time)
    frame_count = frame_count + 1
    prev_rcv_time = yarp.Time_now()

    if (yarp.Time_now() - prev_monitor_time) > 5.0:
        prev_monitor_time = yarp.Time_now()
        if frame_count > 0:
            frame_rate = 1.0 / (frame_jitter / frame_count)
        else:
            frame_rate = 0.0
        frame_jitter = 0
        frame_count = 0
        print(f"Frame rate: {frame_rate}")

        if frame_rate < 28.0:
            print(f"QOS low! frame rate = {frame_rate:.0f}")

            cmd = yarp.Bottle()
            cmd.fromString("prop set /view (qos ((tos 16)))")
            print(cmd.toString())
            admin_port.write(cmd)

            cmd.clear()
            cmd.fromString("prop set /view (sched ((priority 10) (policy 1)))")
            print(cmd.toString())
            admin_port.write(cmd)

    print("QOS monitor accepted data")

    return True
