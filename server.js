var http = require("http");
var fs = require("fs");
var path = require('path');

var cache = {};

var mime = {
    ".css": "text/css",
    ".js": "application/javascript",
    ".png": "image/png",
    ".html": "text/html"
};

http.createServer(function(request, response) {
    if (request.method !== "GET")
        return;

    var fileName = request.url === "/" ? "./index.html" : "." + request.url;

    function respond(buffer) {
        if (buffer === null)
            response.statusCode = 404;
        else {
            response.writeHeader(200, {
                "Content-Type": mime[path.extname(fileName)]
            });

            response.write(buffer);
        }

        response.end();
    }

    if (cache[fileName] === undefined)
        fs.readFile(fileName, function(error, buffer) {
            if (error)
                cache[fileName] = null;
            else
                respond(cache[fileName] = buffer);
        });
    else
        respond(cache[fileName]);
}).listen(8080);