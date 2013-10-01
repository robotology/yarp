
-- loading lua-yarp binding library
require("yarp")


-- declaring 'PortMonitor' object as an empty table
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

    return
end


--
-- getparan is called on getCarrierParams by the port administrator
-- @return property The Property
--
PortMonitor.getparam = function() 

    return property
end


