--
-- Copyright (C) 2012 IITRBCS
-- Authors: Ali Paikan
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--
 
-- loading lua-yarp binding library
require("yarp")



--
-- create is called when the port monitor is created 
-- @return Boolean
--
PortMonitor.create = function()
    -- set the constraint here 
    PortMonitor.setConstraint("false")       
    switchDisplay = false;
    return true;
end



-- 
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    -- unset the all events on destroy
    PortMonitor.unsetEvent("e_line_selected")
    PortMonitor.unsetEvent("e_ball_selected")
end



--
-- accept is called when the port receives new data
-- @param reader The ConnectionReader
-- @return Boolean
-- if false is returned, the data will be ignored 
-- and update() will never be called
PortMonitor.accept = function(reader)
    print("selector: PortMonitor.accept()")
    if switchDisplay == true then
        switchDisplay = false
        PortMonitor.unsetEvent("e_line_selected")
        PortMonitor.setEvent("e_ball_selected")
    else
        switchDisplay = true
        PortMonitor.setEvent("e_line_selected")
        PortMonitor.unsetEvent("e_ball_selected")
    end
    return true
end

