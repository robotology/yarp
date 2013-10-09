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
    print("on create")
    PortMonitor.img_in = yarp.ImageRgb()
    PortMonitor.img_out = yarp.ImageRgb()
    PortMonitor.index = 0;
    PortMonitor.bdraw = true
    PortMonitor.bg = {r=255, g=255, b=255}
    PortMonitor.fg = {r=255, g=0, b=0}
    PortMonitor.time = yarp.Time_now()    
    return true;
end



-- 
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    print("on destroy")
end



--
-- update is called when the port receives new data
-- @param reader The ConnectionReader
--
PortMonitor.update = function(reader)
    PortMonitor.img_in:read(reader)
    PortMonitor.img_out:copy(PortMonitor.img_in)

    w = PortMonitor.img_out:width()
    h = PortMonitor.img_out:height()
    r = math.min(w,h) / 2
    r = r - r/4
    for i=0,2*math.pi,math.pi/32 do
        x = math.floor(w/2 + r * math.cos(i))
        y = math.floor(h/2 + r * math.sin(i))
        if PortMonitor.bdraw == true then
            PortMonitor.img_out:pixel(x, y).r = PortMonitor.bg.r
            PortMonitor.img_out:pixel(x, y).g = PortMonitor.bg.g
            PortMonitor.img_out:pixel(x, y).b = PortMonitor.bg.b
        else
            PortMonitor.img_out:pixel(x, y).r = PortMonitor.fg.r
            PortMonitor.img_out:pixel(x, y).g = PortMonitor.fg.g
            PortMonitor.img_out:pixel(x, y).b = PortMonitor.fg.b
        end            
    end
 
    for i=0,PortMonitor.index,math.pi/32 do
        x = math.floor(w/2 + r * math.cos(i))
        y = math.floor(h/2 + r * math.sin(i))
        if PortMonitor.bdraw == true then
            PortMonitor.img_out:pixel(x, y).r = PortMonitor.fg.r            
            PortMonitor.img_out:pixel(x, y).g = PortMonitor.fg.g            
            PortMonitor.img_out:pixel(x, y).b = PortMonitor.fg.b
        else
            PortMonitor.img_out:pixel(x, y).r = PortMonitor.bg.r
            PortMonitor.img_out:pixel(x, y).g = PortMonitor.bg.g
            PortMonitor.img_out:pixel(x, y).b = PortMonitor.bg.b
        end            
    end
    
    t = yarp.Time_now()
    if (t-PortMonitor.time) >= 0.1 then
        if PortMonitor.index >= 2*math.pi then            
            PortMonitor.index = 0
            if PortMonitor.bdraw == true then
                PortMonitor.bdraw = false                
            else
                PortMonitor.bdraw = true
            end
        else
            PortMonitor.index = PortMonitor.index + math.pi/32
        end            
        PortMonitor.time = t
    end

    con = yarp.DummyConnector()
    PortMonitor.img_out:write(con:getWriter())
    return con:getReader()
end


--
-- setparam is called on setCarrierParams by the port administrator  
-- @param property The Property
--
PortMonitor.setparam = function(property) 
    bt = property:findGroup("bg", "background color")
    if bt:isNull() ~= true then
        if bt:size() >=4  then
            PortMonitor.bg.r = bt:get(1):asInt()
            PortMonitor.bg.g = bt:get(2):asInt()
            PortMonitor.bg.b = bt:get(3):asInt()
        end
    end
    bt = property:findGroup("fg", "forground color")
    if bt:isNull() ~= true then
        if bt:size() >=4  then
            PortMonitor.fg.r = bt:get(1):asInt()
            PortMonitor.fg.g = bt:get(2):asInt()
            PortMonitor.fg.b = bt:get(3):asInt()
        end
    end
end


--
-- getparan is called on getCarrierParams by the port administrator
-- @return property The Property
--
PortMonitor.getparam = function() 

    property = yarp.Property()
    bt = yarp.Bottle()
    bg = bt:addList()    
    bg:addString("bg")
    bg:addInt(PortMonitor.bg.r)
    bg:addInt(PortMonitor.bg.g)
    bg:addInt(PortMonitor.bg.b)
    fg = bt:addList()    
    fg:addString("fg")
    fg:addInt(PortMonitor.fg.r)
    fg:addInt(PortMonitor.fg.g)
    fg:addInt(PortMonitor.fg.b)
    property:fromString(bt:toString())
    return property
end


