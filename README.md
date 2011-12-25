Evented Adder Server samples 
=====
Compare luvit, node, libuv, libev by implementing simple REPL server which keeps adding integer numbers given by client.



Sub modules
=====
1. git clone first, and update all submodules.
2. git submodule init
3. git submodule update           Then you get libev and libuv

build libev
----
1. cd ev-adder/deps/libev
2. ./configure
3. make

build libuv
----
1. cd uv-adder/deps/libuv
2. make 


How To Use
======
see source code. it's dead simple.



Performance summary (on OSX Lion i5 2.53GHz), concurrency:10 or 20
======

1. ev client to ev server (text): about 75K query/sec
2. uv client to uv server (text) : about 70K query/sec
3. uv client to luvit server (text) : about 53K query/sec
4. luvit client to luvit server (msgpack) : about 32K query/sec
5. luvit client to luvit server (text) : about 30K query/sec
6. uv client to node server (text) : about 21K query/sec

Conclusion
=====
luvit + msgpack looks enough fast to implement heavy duty messaging server.

About Msgpack
=====
This test is using modified version of 
[luajit-msgpack-pure](https://github.com/catwell/luajit-msgpack-pure) 
because luvit doesn't have default modules like math, string, table.



