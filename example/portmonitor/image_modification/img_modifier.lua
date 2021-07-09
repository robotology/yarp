-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")

--
-- PortMonitor table is used by portmonitor_carrier
-- to invoke the corresponding methods.The methods are
-- optional but must satisfy the following format:
--
-- PortMonitor.create = function(options) ... return true end,
-- PortMonitor.destroy = function() ... end,
-- PortMonitor.accept = function(thing) ... return true end,
-- PortMonitor.update = function(thing) ... return thing end,
-- PortMonitor.setparam = function(param) ... end,
-- PortMonitor.getparam = function() ... return param end
-- PortMonitor.trig = function() ... return end
--

--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
    print("on create")
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
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
    return true
end



--
-- trig is called when one of the peer portmonitors
-- to the same input port receives data. this is
-- called before the update() method of the peer
-- portmoniotr is invoked
--
PortMonitor.trig = function()
    --print("trigged!")
end



--
-- update is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Things
PortMonitor.update = function(thing)
    img_out = thing:asImageOfPixelRgb()
    if img_out == nil then
        print("img_modifier.lua: got wrong data type (expected type ImageOfPixelRgb)")
        return thing
    end

    --img_out:copy(img_in)

    w = img_out:width()
    h = img_out:height()
    r = math.min(w,h) / 2
    r = r - r/4
    for i=0,2*math.pi,math.pi/32 do
        x = math.floor(w/2 + r * math.cos(i))
        y = math.floor(h/2 + r * math.sin(i))
        if PortMonitor.bdraw == true then
            img_out:pixel(x, y).r = PortMonitor.bg.r
            img_out:pixel(x, y).g = PortMonitor.bg.g
            img_out:pixel(x, y).b = PortMonitor.bg.b
        else
            img_out:pixel(x, y).r = PortMonitor.fg.r
            img_out:pixel(x, y).g = PortMonitor.fg.g
            img_out:pixel(x, y).b = PortMonitor.fg.b
        end
    end

    for i=0,PortMonitor.index,math.pi/32 do
        x = math.floor(w/2 + r * math.cos(i))
        y = math.floor(h/2 + r * math.sin(i))
        if PortMonitor.bdraw == true then
            img_out:pixel(x, y).r = PortMonitor.fg.r
            img_out:pixel(x, y).g = PortMonitor.fg.g
            img_out:pixel(x, y).b = PortMonitor.fg.b
        else
            img_out:pixel(x, y).r = PortMonitor.bg.r
            img_out:pixel(x, y).g = PortMonitor.bg.g
            img_out:pixel(x, y).b = PortMonitor.bg.b
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

    return thing
end


--
-- setparam is called on setCarrierParams by the port administrator
-- @param property The Property
--
PortMonitor.setparam = function(property)
    bt = property:findGroup("bg", "background color")
    if bt:isNull() ~= true then
        if bt:size() >=4  then
            PortMonitor.bg.r = bt:get(1):asInt32()
            PortMonitor.bg.g = bt:get(2):asInt32()
            PortMonitor.bg.b = bt:get(3):asInt32()
        end
    end
    bt = property:findGroup("fg", "forground color")
    if bt:isNull() ~= true then
        if bt:size() >=4  then
            PortMonitor.fg.r = bt:get(1):asInt32()
            PortMonitor.fg.g = bt:get(2):asInt32()
            PortMonitor.fg.b = bt:get(3):asInt32()
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
    bg:addInt32(PortMonitor.bg.r)
    bg:addInt32(PortMonitor.bg.g)
    bg:addInt32(PortMonitor.bg.b)
    fg = bt:addList()
    fg:addString("fg")
    fg:addInt32(PortMonitor.fg.r)
    fg:addInt32(PortMonitor.fg.g)
    fg:addInt32(PortMonitor.fg.b)
    property:fromString(bt:toString())
    return property
end
