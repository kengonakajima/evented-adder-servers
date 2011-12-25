local tcp = require('tcp')
local os = require('os')

local mp = require "./luajit-msgpack-pure"


local trialNum = 100000
local connNum = 20
local startAt = os.clock()

local finished=0


function newclient()
   local client = tcp.new()
   client:connect("127.0.0.1", 8090)

   client:on("complete", 
             function()
                print("connected:",client)
                client.cnt = 0 
                client:read_start()
                client:write( mp.pack( { numToAdd=1}),
                              function(err)                              
                                 if err then print("err:",err) end
                              end )
             end)

   client:on("data",
             function(chunk)
                client.cnt = client.cnt + 1
                if (client.cnt%2000)==0 then print("cnt:", client.cnt) end
                if client.cnt >= trialNum then
                   client:close( function() p("closed.") end )
                   finished = finished + 1
                   if finished == connNum then
                      local endAt = os.clock()
                      local dt = endAt - startAt
                      p( "consumed time:", dt, " avg:", ( trialNum*connNum / dt ) , " q/sec" )
                   end
                else
                   local sum = tonumber(chunk)
                   client:write( mp.pack( {numToAdd= client.cnt } ), function(err) if err then p("w err:", err) end end)
                end
             end)

end


--------------------------------





for i=1,connNum do
   newclient()
end
