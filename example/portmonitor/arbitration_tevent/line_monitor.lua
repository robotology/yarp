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
PortMonitor.create = function(options)
    -- set the constraint here 
    PortMonitor.setConstraint("not e_ball")
    return true;
end


