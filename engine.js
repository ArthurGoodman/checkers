"use strict";

function Engine(board) {
    var webSocket = null;

    var messages = [];
    var lock = false;

    function processMessage(message) {
        messages.push(message);
    }

    function sendMessage(message) {
        if (webSocket != null)
            webSocket.send(message);
    }

    this.start = function start() {
        var serverLocation = "ws://" + (location.hostname.length > 0 ? location.hostname : "localhost") + ":443";

        webSocket = new WebSocket(serverLocation);

        webSocket.onopen = function(e) {};

        webSocket.onclose = function(e) {
            setTimeout(start, 5000);
        };

        webSocket.onmessage = function(e) {
            console.log(e.data);
            processMessage(JSON.parse(e.data));
        };

        webSocket.onerror = function(e) {};
    }

    this.click = function(x, y) {
        if (lock || board.isAnimating() || board.isNull())
            return;

        sendMessage(board.map(x, y));
    }

    this.reset = function() {
        sendMessage(-1);
    }

    this.tick = function() {
        board.draw();

        if (!lock && !board.isAnimating() && messages.length > 0) {
            var message = messages.shift();

            switch (message.cmd) {
                case "board":
                    board.deselect();
                    board.setData(message.data);
                    break;

                case "select":
                    board.select(message.index);
                    break;

                case "highlight":
                    board.highlight(message.cells);
                    break;

                case "move":
                    board.animate(message.from, message.to);
                    break;

                case "remove":
                    board.remove(message.index);
                    break;

                case "winner":
                    lock = true;
                    setTimeout(function() {
                        alert(message.winner == "" ? "Draw!" : (message.winner == "x" ? "AI" : "Player") + " won!");
                        lock = false;
                    }, 100);
                    break;
            }
        }
    }
}