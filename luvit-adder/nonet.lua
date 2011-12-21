local os = require('os')

function makedummycli()
   local cl={}
   cl.total = 0
   cl.on_read = function(self,chunk)
                   local n = tonumber(chunk)
                   self.total = self.total + n
                   return self.total
                end
   return cl
end

local trialNum=100000
local cliNum=30
clients={}

local startAt = os.clock()

for i=1,cliNum do
   clients[i] = makedummycli()
end


for i=1,trialNum do
   local s = tostring(i) .. "\n"
   for j=1,cliNum do
      clients[j]:on_read(s)
   end
end

local endAt = os.clock()
local dt = endAt - startAt 
p( "elapsedTime:", dt, " avg:", (cliNum*trialNum) / dt, " q/sec" )
