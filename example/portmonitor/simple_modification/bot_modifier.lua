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
    print("in create!")
    return true;
end



--
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    print("in destroy!")
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
    if thing:asBottle() == nil then
        print("bot_modifier.lua: got wrong data type (expected type Bottle)")
        return false
    end
    if thing:asBottle():toString() == "ignore" then
        return false
    end
    return true
end


--
-- trig is called when one of the peer portmonitors
-- to the same input port receives data. This is
-- called before the update() method of the peer
-- portmoniotr is invoked
--
PortMonitor.trig = function()
end


--
-- update is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Things
PortMonitor.update = function(thing)
    if thing:asBottle() == nil then
        print("bot_modifier.lua: got wrong data type (expected type Bottle)")
        return thing
    end

    bt = thing:asBottle()
    bt:addString("modified from Lua :)")
    return thing
end


--
-- setparam is called on setCarrierParams by the port administrator
-- @param property The Property
--
PortMonitor.setparam = function(property)

end


--
-- getparan is called on getCarrierParams by the port administrator
-- @return property The Property
--
PortMonitor.getparam = function()

    return property
end
