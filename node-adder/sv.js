var net = require('net');
var sys = require('sys');
var server = net.createServer( function (stream) {
        stream.on('end', function () {
                stream.end();
            });

        stream.on('connect', function () {
                sys.puts("connect");
                stream.total=0;
            });
        stream.on('data', function (data) {
                var n = parseInt(data);
                stream.total += n;
                //                sys.puts( "num:" + n );
                stream.write("" + stream.total + "\n");                
            });
    });

server.listen(8090, 'localhost');
