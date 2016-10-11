var spawn = require("child_process").spawn;
var child = spawn("release/checkers.exe");

child.stdout.on("data", function(data) {
    console.log(data.toString());
});

child.on("close", function(code) {
    console.log("checkers.exe exited with code " + code);
});

var http = require("http");
var fs = require("fs");
var path = require("path");

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
            respond(cache[fileName] = error ? null : buffer);
        });
    else
        respond(cache[fileName]);
}).listen(8080);