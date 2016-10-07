var websocket = null;
var canvas = $("#checkers")[0];
var DrawContext = canvas.getContext("2d");

canvas.addEventListener("click", function(e) {
    var x, y;

    if (e.pageX != undefined && e.pageY != undefined) {
        x = e.pageX;
        y = e.pageY;
    } else {
        x = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
        y = e.clientY + document.body.scrollTop + document.documentElement.scrollTop;
    }

    x -= canvas.offsetLeft;
    y -= canvas.offsetTop;

    console.log("(", x, ",", y, ")");
});

$(document).ready(function initWebSocket() {
    try {
        if (typeof MozWebSocket == "function")
            WebSocket = MozWebSocket;

        if (websocket && websocket.readyState == 1)
            websocket.close();

        websocket = new WebSocket("ws://localhost:43567");

        websocket.onopen = function(evt) {};

        websocket.onclose = function(evt) {};

        websocket.onmessage = function(evt) {
            console.log("message:", evt.data);
            processMessage(JSON.parse(evt.data));
        };

        websocket.onerror = function(evt) {};
    } catch (exception) {
        console.log("error: ", exception);
    }

    draw();
})

function processMessage(message) {
    switch (message.command) {
        case "board":
            break;
    }
}

function sendMessage(message) {
    if (websocket != null)
        websocket.send(JSON.stringify(message));
}

function drawBackground() {
    DrawContext.clearRect(0, 0, canvas.width, canvas.height);

    for (var i = 0; i < 8; i++)
        for (var j = 0; j < 8; j++) {
            DrawContext.fillStyle = (i + j) % 2 == 0 ? "#eee" : "#ddd";
            DrawContext.fillRect(i * canvas.width / 8, j * canvas.height / 8, canvas.width / 8, canvas.height / 8);
        }
}

function draw() {
    drawBackground();
}