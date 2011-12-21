simplest TCP server performance test
=====================

 1. start addsv.lua  (server process)
 2. start addcl.lua  (client process)

 addcl creates many TCP connections to addsv, and counts total query/sec :

"consumed time:"	26.71	" avg:"	37439.161362785	" q/sec"


 You can test the server by " telnet localhost 8080" and type some integers by hand.
 addsv.lua is a simple REPL that counts up the input number.

 This is a skeleton program of simplest RPC server and you can see
 how is the baseline of performance as a RPC server.
 By adding byte stream parser and some RPC routing, you will get more overhead from it.

 In my macbook pro (i5 2.53GHz), it can call about 36K query/sec.

 
 