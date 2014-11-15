--
-- Copyright (C) 2012 IITRBCS
-- Authors: Ali Paikan
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--
 
-- loading lua-yarp binding library
require("yarp")

shouldAccept = true
prevTime = yarp.Time_now()

--
-- create is called when the port monitor is created 
-- @return Boolean
--
PortMonitor.create = function()
    -- set the constraint here 
    PortMonitor.setConstraint("true")
    return true;
end

--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored 
-- and update() will never be called
PortMonitor.accept = function(thing)

    if (yarp.Time_now() - prevTime) > 5.0 then        
        if shouldAccept == true then 
            print('Ball monitor: switching off!')
            shouldAccept = false 
        else
            print('Ball monitor: switching on!')
            shouldAccept = true;
        end    
        prevTime = yarp.Time_now()
    end

    if shouldAccept == true then
        PortMonitor.setEvent("e_ball", 0.5)     -- the event will be expired after 500ms
        return true
    end
    return false
end

