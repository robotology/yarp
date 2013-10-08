--
-- Copyright (C) 2012 IITRBCS
-- Authors: Ali Paikan
-- CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
--
 
-- loading lua-yarp binding library
require("yarp")

--
-- PortMonitor table is used by portmonitor_carrier 
-- to invoke the corresponding methods.The methods are
-- optional but must satisfy the following format:
--
-- PortMonitor = {
--      create = function() ... return true end, 
--      destroy = function() ... end, 
--      update = function(reader) ... return reader end, 
--      setparam = function(param) ... end, 
--      getparam = function() ... return param end
-- }
--
-- declaring 'PortMonitor' object as an empty table
--
PortMonitor = {}



--
-- create is called when the port monitor is created 
-- @return Boolean
--
PortMonitor.create = function()
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
-- update is called when the port receives new data
-- @param reader The ConnectionReader
--
PortMonitor.update = function(reader)
    bt = yarp.Bottle()
    bt:read(reader)
    bt:addString("modified from Lua")
    con = yarp.DummyConnector()
    bt:write(con:getWriter())    
    return con:getReader()
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


