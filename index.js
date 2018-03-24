"use strict";

const spawn = require("child_process").spawn;
const http = require("http");
const fs = require("fs");

var settings = require("./settings.json");

var env = Object.create(process.env)
env.SWI_HOME_DIR="/usr/lib/swi-prolog/"

var child = spawn("./build/checkers", [settings.depth], { env: env });

child.stdout.on("data", function(data) {
    process.stdout.write(data);
});

child.on("close", function(code) {
    console.log("checkers exited with code", code);
});

var cache = {};

http.createServer(function(req, res) {
    var url = req.url == "/" ? "./index.html" : "." + req.url;

    function respond(buffer) {
        if (buffer == null) {
            res.statusCode = 404;
            res.end();
        } else {
            res.writeHeader(200);
            res.end(buffer);
        }
    }

    if (cache[url] == undefined)
        fs.readFile(url, function(error, buffer) {
            respond(cache[url] = error ? null : buffer);
        });
    else
        respond(cache[url]);
}).listen(80);
