-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")



--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
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
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
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
