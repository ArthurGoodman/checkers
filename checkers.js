var BoardDim = 8;
var BoardSize = null;
var CellSize = null;

var websocket = null;

var canvas = $("#checkers")[0];
var ctx = canvas.getContext("2d");

var board = null;

var selection = null;
var target = null;
var animation = null;

var lock = false;

onResize();

function indexAt(x, y) {
    return y * BoardDim + x;
}

function getX(index) {
    return index % BoardDim;
}

function getY(index) {
    return Math.floor(index / BoardDim);
}

function at(x, y) {
    return board[indexAt(x, y)];
}

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

    click(Math.floor(x / BoardSize * BoardDim), Math.floor(y / BoardSize * BoardDim));
});

function onResize() {
    BoardSize = Math.min(window.innerWidth, window.innerHeight) - 50;

    canvas.width = BoardSize;
    canvas.height = BoardSize;

    CellSize = BoardSize / BoardDim;
}

$(document).ready(function() {
    try {
        if (typeof MozWebSocket == "function")
            WebSocket = MozWebSocket;

        if (websocket && websocket.readyState == 1)
            websocket.close();

        websocket = new WebSocket("ws://localhost:43567");

        websocket.onopen = function(e) {};

        websocket.onclose = function(e) {};

        websocket.onmessage = function(e) {
            console.log(e.data);
            processMessage(JSON.parse(e.data));
        };

        websocket.onerror = function(e) {};
    } catch (exception) {
        console.log("error: ", exception);
    }

    window.requestAnimationFrame(draw);
})

function processMessage(message) {
    switch (message.cmd) {
        case "board":
            board = message.board;
            break;

        case "unlock":
            lock = false;
            break;

        case "move":
            selection = message.from;
            target = message.to;
            animation = 0;
            break;
    }
}

function sendMessage(message) {
    if (websocket != null)
        websocket.send(JSON.stringify(message));
}

function click(x, y) {
    if (lock || animation != null || board == null)
        return;

    if (at(x, y) == "o" || at(x, y) == "p") {
        if (selection == indexAt(x, y))
            selection = null;
        else
            selection = indexAt(x, y);
    } else if (selection != null) {
        if (at(x, y) == "e")
            makeMove(selection, indexAt(x, y));

        selection = null;
    }
}

function makeMove(from, to) {
    sendMessage({
        cmd: "move",
        from: from,
        to: to
    });

    lock = true;
}

function drawBackground() {
    ctx.clearRect(0, 0, BoardSize, BoardSize);

    for (var x = 0; x < BoardDim; x++)
        for (var y = 0; y < BoardDim; y++) {
            ctx.fillStyle = (x + y) % 2 == 0 ? "#eee" : "#ddd";
            ctx.fillRect(x * CellSize, y * CellSize, CellSize + 1, CellSize + 1);
        }
}

function drawBoard() {
    if (board == null)
        return;

    function drawCircle(x, y, radius, lineWidth, color, fill) {
        ctx.beginPath();
        ctx.lineWidth = lineWidth;
        ctx.arc(x, y, radius, 0, Math.PI * 2, false);
        ctx.closePath();

        if (fill) {
            ctx.fillStyle = color;
            ctx.fill();
        } else {
            ctx.strokeStyle = color;
            ctx.stroke();
        }
    }

    function drawPiece(x, y, color, king) {
        var fill = selection == indexAt(x, y);

        x = (x + 0.5) * CellSize;
        y = (y + 0.5) * CellSize;

        if (animation != null && fill) {
            x = x * (1 - animation) + (getX(target) + 0.5) * CellSize * animation;
            y = y * (1 - animation) + (getY(target) + 0.5) * CellSize * animation;
        }

        var radius = CellSize / 2 / 1.2;

        drawCircle(x, y, radius, 3, color, fill);

        if (king) {
            drawCircle(x, y, radius, 3, color);
            drawCircle(x, y, radius / 1.3, 3, color);
            drawCircle(x, y, radius / 1.9, 3, color);
            drawCircle(x, y, radius / 3.5, 3, color);
        }
    }

    for (var x = 0; x < BoardDim; x++)
        for (var y = 0; y < BoardDim; y++)
            if (at(x, y) != "n" && at(x, y) != "e")
                drawPiece(x, y, (at(x, y) == "x" || at(x, y) == "y") ? "#e33" : "#33e", at(x, y) == "y" || at(x, y) == "p");

    if (animation != null) {
        animation += 0.1;

        if (animation >= 1) {
            board[target] = board[selection];
            board[selection] = "e";

            selection = null;
            target = null;
            animation = null;
        }
    }
}

function drawFrame() {
    ctx.lineWidth = 1;
    ctx.strokeStyle = "#000";
    ctx.beginPath();
    ctx.rect(0, 0, BoardSize, BoardSize);
    ctx.closePath();
    ctx.stroke();
}

function draw() {
    drawBackground();
    drawBoard();
    drawFrame();

    window.requestAnimationFrame(draw);
}