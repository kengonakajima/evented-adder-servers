local tcp = require('tcp')
local string = require('string')
local mp = require "./luajit-msgpack-pure"


function dump(t)
   for k,v in pairs(t) do
      print("k:", k, " v:", v )
   end
end


local server = tcp.create_server("0.0.0.0", 8090, function (client)
  p("on_connection", client)

  client.total = 0 -- client has state
  
  client:on("data", function (chunk)
                       
                       local offset,res = mp.unpack(chunk) 
                       assert(offset)
--                       print("data. chunklen:", string.len(chunk), " offset:", offset )
--                       dump(res)
                       client.total = client.total + res.numToAdd
                       client:write( mp.pack( { totalNum= client.total}) )
                    end)


  client:on("end", function ()
    p("on_end. closing..")
    client:close(function ()  p("on_closed") end)
  end)
  
end)

server:on("error", function (err)  p("ERROR", err) end)

print("simplest adder server")

