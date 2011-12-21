simplest TCP server performance test
=====================

 1. luvit addsv.lua  (server process)
 2. luvit addcl.lua  (client process)

 addcl.lua creates many TCP connections to addsv.lua, and it counts total query/sec :

"consumed time:"	26.71	" avg:"	37439.161362785	" q/sec"

 addsv.lua is a simple REPL that counts up the input number.

 You can test the server by " telnet localhost 8080" and type some integers by hand.
 
 This is a skeleton program of simplest RPC server and you can see
 how is the baseline of performance as a RPC server.
 
 By adding byte stream parser and some RPC routing, you will get more overhead from it.
 You can compare the result with nonet.lua, that doesn't do any network things.
 
 In my macbook pro (i5 2.53GHz), addsv-addcl pair can call about 36K query/CPUsec,
 and nonet.lua averages 25M query/CPUsec. Note that it uses os.clock().
 
 By adding protocol parsers and routings, it'll get 10K q/CPUsec or something,
 but it's too slow for MMOG servers.

 Now I'm wondering  500K or 1M q/CPUsec is possible or not on luvit. how?
 
 