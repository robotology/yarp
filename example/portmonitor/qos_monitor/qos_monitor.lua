-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")

--
-- PortMonitor table is used by portmonitor_carrier
-- to invoke the corresponding methods.The methods are
-- optional but must satisfy the following format:
--
--  PortMonitor.create = function(options) ... return true end,
--  PortMonitor.destroy = function() ... end,
--  PortMonitor.accept = function(thing) ... return true end,
--  PortMonitor.update = function(thing) ... return thing end,
--  PortMonitor.setparam = function(param) ... end,
--  PortMonitor.getparam = function() ... return param end
--  PortMonitor.trig = function() ... return end
--

--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
    print("[qos_monitor] Qos monitor started (rate: 30 ms)")
    PortMonitor.setTrigInterval(0.033)
    return true;
end



--
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    print("[qos_monitor] Qos monitor stoped!")
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
    received = true
    return true
end


--
-- In this example, trig() is called periodically every 30ms
-- This is specified in the PortMonitor.create() using
-- PortMonitor.setTrigInterval(0.3).
--
PortMonitor.trig = function()
    if received == false then
        print("[qos_monitor] Qos is not okay. (data frequency < 30Hz)")
    else
        received = false
    end
end
