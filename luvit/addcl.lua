local tcp = require('tcp')
local os = require('os')

local trialNum = 10000
local connNum = 30
local startAt = os.clock()

local finished=0

function newclient()
   local client = tcp.new()
   client:connect("127.0.0.1", 8080)

   client:on("complete", 
             function()
                print("connected:",client)
                client.cnt = 0 
                client:read_start()
                client:write( "1\n",
                              function(err)                              
                                 if err then print("err:",err) end
                              end )
             end)

   client:on("data",
             function(chunk)
                client.cnt = client.cnt + 1
                if (client.cnt%1000)==0 then print("cnt:", client.cnt) end
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
                   client:write( tostring(client.cnt) .. "\n", function(err) if err then p("w err:", err) end end)
                end
             end)

end


--------------------------------





for i=1,connNum do
   newclient()
end
