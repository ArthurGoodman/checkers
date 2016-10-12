var BoardDim = 8;
var BoardSize = null;
var CellSize = null;

var ws = null;

var canvas = $("#checkers")[0];
var ctx = canvas.getContext("2d");

var board = null;

var selection = null;
var target = null;
var animation = null;

var highlights = [];

var messages = [];

var lock = false;

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

$(canvas).on("click", function(e) {
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

$(window).on("resize", function() {
    BoardSize = Math.min(window.innerWidth, window.innerHeight) - 70;

    canvas.width = BoardSize;
    canvas.height = BoardSize;

    CellSize = BoardSize / BoardDim;
});

$(window).resize();

$("#reset").on("click", function() {
    sendMessage(-1);
});

$(document).ready(function() {
    var serverLocation = "ws://" + (location.hostname.length > 0 ? location.hostname : "localhost") + ":43567";

    function startWebSocket() {
        ws = new WebSocket(serverLocation);

        ws.onopen = function(e) {};

        ws.onclose = function(e) {
            setTimeout(startWebSocket, 1000);
        };

        ws.onmessage = function(e) {
            console.log(e.data);
            processMessage(JSON.parse(e.data));
        };

        ws.onerror = function(e) {};
    }

    startWebSocket();

    window.requestAnimationFrame(frame);
})

function processMessage(message) {
    messages.unshift(message);
}

function sendMessage(message) {
    if (ws != null)
        ws.send(message);
}

function click(x, y) {
    if (lock || animation != null || board == null)
        return;

    sendMessage(indexAt(x, y).toString());
}

function drawBackground() {
    ctx.clearRect(0, 0, BoardSize, BoardSize);

    for (var x = 0; x < BoardDim; x++)
        for (var y = 0; y < BoardDim; y++) {
            ctx.fillStyle = highlights.indexOf(indexAt(x, y)) !== -1 ? "#afa" : (x + y) % 2 == 0 ? "#eee" : "#ddd";
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

    function processCell(x, y) {
        if (at(x, y) != "n" && at(x, y) != "e")
            drawPiece(x, y, (at(x, y) == "x" || at(x, y) == "y") ? "#e33" : "#33e", at(x, y) == "y" || at(x, y) == "p");
    }

    for (var x = 0; x < BoardDim; x++)
        for (var y = 0; y < BoardDim; y++)
            if (selection != indexAt(x, y))
                processCell(x, y);

    if (selection != null)
        processCell(getX(selection), getY(selection));

    if (animation != null) {
        animation += 0.05;

        if (animation >= 1) {
            if (getY(target) == 7 && board[selection] == "x")
                board[target] = "y";
            else if (getY(target) == 0 && board[selection] == "o")
                board[target] = "p";
            else
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

function frame() {
    drawBackground();
    drawBoard();
    drawFrame();

    if (!lock && animation == null && messages.length > 0) {
        var message = messages.pop();

        switch (message.cmd) {
            case "board":
                selection = null;
                board = message.board;
                break;

            case "select":
                selection = message.index;
                break;

            case "highlight":
                highlights = message.cells;
                break;

            case "move":
                highlights = [];
                selection = message.from;
                target = message.to;
                animation = 0;
                break;

            case "remove":
                board[message.index] = "e";
                break;

            case "winner":
                lock = true;
                setTimeout(function() {
                    alert((message.winner == "x" ? "AI" : "Player") + " won!");
                    lock = false;
                }, 100);
                break;
        }
    }

    window.requestAnimationFrame(frame);
}