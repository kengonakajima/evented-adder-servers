local tcp = require('tcp')
local string = require('string')

local server = tcp.create_server("0.0.0.0", 8080, function (client)
  p("on_connection", client)

  client.total = 0 -- client has state
  
  client:on("data", function (chunk)
    local newline = string.find( chunk, "\n")
    if newline then
       local n = tonumber(chunk)
       if n then
          client.total = client.total + n
       end
       client:write( tostring( client.total) .. "\n", nil) 
    end

  end)

  client:on("end", function ()
    p("on_end. closing..")
    client:close(function ()  p("on_closed") end)
  end)
  
end)

server:on("error", function (err)  p("ERROR", err) end)

print("simplest adder server at 8080")

