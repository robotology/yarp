-- Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- All rights reserved.
--
-- This software may be modified and distributed under the terms of the
-- BSD-3-Clause license. See the accompanying LICENSE file for details.


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
