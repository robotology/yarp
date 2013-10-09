
Port Monitor carrier 
---------------------

- What is the port monitor?

  It is a run-time monitor object implemented as a new carrier which 
  allows to modify incoming (in future also outgoing) data to a port 
  using user script. Currently only Lua is supported by the port monitor.  


- What i need to use port monitor? 

  You need to compile yarp-lua binding and set the 'LUA_CPATH' to find 
  yarp-lua binding library. 


- How it works? 

  You should connect two ports using yarp 'portmonitor' carrier modifier:
  
  $ yarp connect /out /in tcp+recv.portmonitor+script.lua+context.myapp+file.my_lua_script
  
  'script.lua' tells the carrier to create a port monitor object for Lua.  
  'context.myapp' tells the resource finder to load the script from the 'myapp' context. 
  'file.my_lua_script' indicates 'my_lua_script' should be loaded by monitor object. 
   For now, 'my_lua_script' should be the complete path to the script file. the 
   postfix (e.g., '.lua') is not necessary.  
 
  When data arrive to an input port, the port monitor will call the corresponding 
  handler (i.e., PortMonitor.update) from lua script and passes an instance of 
  connection reader to the handler. Using the connection reader, user script
  can access the data, modify it and return it to the port monitor object. 
  user script in lua should have a global 'PortMonitor' table which satisfies 
  the following format : 

  PortMonitor = {
       create = function() ... return true end, 
       destroy = function() ... end, 
       update = function(reader) ... return reader end, 
       setparam = function(param) ... end, 
       getparam = function() ... return param end
  }
  
  Port monitor carrier looks for a global table name 'PortMonitor' in the user 
  script and calls its corresponding functions (see the examples).   
   
 
