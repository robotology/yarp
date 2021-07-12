#!/usr/bin/lua

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause


-- LUA_CPATH should have the path to yarp-lua binding library (i.e. yarp.so, yarp.dll)
require("yarp")

-- initialize yarp network
yarp.Network()

options = yarp.Property()
options:put("device", "remote_controlboard")
options:put("local", "/motor/lua/right_arm")
options:put("remote", "/icubSim/right_arm")

-- setting QoS pereferences for the driver (local and remote)
qos_local = options:addGroup("local_qos")
qos_local:put("thread_priority", 20)
qos_local:put("thread_policy", 1)
qos_local:put("packet_priority", "LEVEL:HIGH")

qos_remote = options:addGroup("remote_qos")
qos_remote:put("thread_priority", 30)
qos_remote:put("thread_policy", 1)
qos_remote:put("packet_priority", "DSCP:AF42")

-- open the driver
driver = yarp.PolyDriver(options)
if driver:isValid() == false then
    print("Cannot open the device");
    os.exit()
end

-- open the interfaces
ipos = driver:viewIPositionControl()
if ipos == nil then
    print("Cannot open the IPositionControl interface");
    driver:close()
    os.exit()
end

icm = driver:viewIControlMode()
if icm == nil then
    print("Cannot open the IControlMode interface");
    driver:close()
    os.exit()
end

-- send position command to motors
for i=1,100 do
    position = math.random(-90, 10)
    print("setting position of joint 0 to "..position)
    icm:setControlMode(0, VOCAB_CM_POSITION)
    ipos:setRefSpeed(0, 30)
    ipos:positionMove(0, position)
    yarp.delay(0.5)
end


driver:close()

-- Deinitialize yarp network
yarp.Network_fini()
