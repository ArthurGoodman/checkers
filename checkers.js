var BoardWidth = 8;
var BoardHeight = 8;

var websocket = null;
var canvas = $("#checkers")[0];
var context = canvas.getContext("2d");

var board = null;
var selection = null;

function indexAt(x, y) {
    return y * BoardWidth + x;
}

function at(x, y) {
    return board[indexAt(x, y)];
}

onResize();

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

    click(Math.floor(x / canvas.width * BoardWidth), Math.floor(y / canvas.height * BoardHeight));
});

function onResize() {
    var size = Math.min(window.innerWidth, window.innerHeight) - 50;

    canvas.width = size;
    canvas.height = size;

    draw();
}

$(document).ready(function() {
    try {
        if (typeof MozWebSocket == "function")
            WebSocket = MozWebSocket;

        if (websocket && websocket.readyState == 1)
            websocket.close();

        websocket = new WebSocket("ws://localhost:43567");

        websocket.onopen = function(evt) {};

        websocket.onclose = function(evt) {};

        websocket.onmessage = function(evt) {
            console.log(evt.data);
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
            board = message.board;
            draw();
            break;
    }
}

function sendMessage(message) {
    if (websocket != null)
        websocket.send(JSON.stringify(message));
}

function click(x, y) {
    if (board == null)
        return;

    if (at(x, y)[0] == "o") {
        if (selection == indexAt(x, y))
            selection = null;
        else
            selection = indexAt(x, y);
    } else if (selection != null) {
        if (at(x, y) == "e" && isValidMove(x, y))
            makeMove(selection, indexAt(x, y));

        selection = null;
    }

    draw();
}

function isValidMove(from, to) {
    return true;
}

function makeMove(from, to) {
    sendMessage({
        command: "move",
        from: from,
        to: to
    });
}

function drawBackground() {
    context.clearRect(0, 0, canvas.width, canvas.height);

    for (var x = 0; x < BoardWidth; x++)
        for (var y = 0; y < BoardHeight; y++) {
            context.fillStyle = (x + y) % 2 == 0 ? "#eee" : "#ddd";
            context.fillRect(x * canvas.width / BoardWidth, y * canvas.height / BoardHeight, canvas.width / BoardWidth + 1, canvas.height / BoardHeight + 1);
        }

    context.lineWidth = 1;
    context.strokeStyle = "#000";
    context.rect(0, 0, canvas.width, canvas.height);
    context.stroke();
}

function drawBoard() {
    if (board == null)
        return;

    function drawCircle(x, y, radius, lineWidth, color, fill) {
        context.beginPath();
        context.lineWidth = lineWidth;
        context.arc(x, y, radius, 0, Math.PI * 2, false);
        context.closePath();

        if (fill) {
            context.fillStyle = color;
            context.fill();
        } else {
            context.strokeStyle = color;
            context.stroke();
        }
    }

    function drawPiece(x, y, color, king) {
        var fill = selection == indexAt(x, y);

        x = (x + 0.5) * canvas.width / BoardWidth;
        y = (y + 0.5) * canvas.height / BoardHeight;

        var radius = Math.min(canvas.width / BoardWidth / 2 / 1.2, canvas.height / BoardHeight / 2 / 1.2);

        drawCircle(x, y, radius, 3, color, fill);

        if (king) {
            drawCircle(x, y, radius, 3, color);
            drawCircle(x, y, radius / 1.3, 3, color);
            drawCircle(x, y, radius / 1.9, 3, color);
            drawCircle(x, y, radius / 3.5, 3, color);
        }
    }

    for (var x = 0; x < BoardWidth; x++)
        for (var y = 0; y < BoardHeight; y++)
            if (at(x, y) != "n" && at(x, y) != "e")
                drawPiece(x, y, at(x, y)[0] == "x" ? "#e33" : "#33e", at(x, y).length == 2);
}

function draw() {
    drawBackground();
    drawBoard();
}