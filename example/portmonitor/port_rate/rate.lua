-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")

--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
    isReceiver = (options:find("sender_side"):asInt32() == 0)
    sourceName = options:find("source"):asString()
    destinationName = options:find("destination"):asString()
    if isReceiver == true then
        print("rate.lua: I will slow down the transmition rate")
    else
        print("rate.lua: I will print the connection rate")
    end
    prev_time = yarp.Time_now()
    return true;
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
PortMonitor.accept = function(thing)

    if isReceiver == true then
        print("\nRate of", sourceName, ":", yarp.Time_now() - prev_time)
        prev_time = yarp.Time_now()
        return true
    end

    -- filter with the desired frequency. e.g, 300ms
    if yarp.Time_now() - prev_time < 0.300 then
        return false
    end

    prev_time = yarp.Time_now()
    return true
end
