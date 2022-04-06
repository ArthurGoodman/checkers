"use strict";

function Board(canvas) {
    var ctx = canvas.getContext("2d");

    const BoardDim = 8;
    var boardSize = null;
    var cellSize = null;

    var data = null;

    var selection = null;
    var target = null;
    var animation = null;

    var highlights = [];

    var redraw = true;

    var images = {
        x: $("#x")[0],
        X: $("#X")[0],
        o: $("#o")[0],
        O: $("#O")[0]
    }

    function indexAt(x, y) {
        return y * BoardDim + x;
    }

    function getX(index) {
        return index % BoardDim;
    }

    function getY(index) {
        return Math.floor(index / BoardDim);
    }

    this.resize = function(size) {
        boardSize = size;

        canvas.width = boardSize;
        canvas.height = boardSize;

        cellSize = boardSize / BoardDim;

        redraw = true;
    }

    this.isAnimating = function() {
        return animation != null;
    }

    this.isNull = function() {
        return data == null;
    }

    this.animate = function(from, to) {
        highlights = [];
        selection = from;
        target = to;
        animation = 0;
    }

    this.map = function(x, y) {
        return indexAt(Math.floor(x / boardSize * BoardDim), Math.floor(y / boardSize * BoardDim));
    }

    this.setData = function(d) {
        data = d;
        redraw = true;
    }

    this.select = function(index) {
        selection = index;
        redraw = true;
    }

    this.remove = function(index) {
        data[index] = "_";
        redraw = true;
    }

    this.highlight = function(cells) {
        highlights = cells;
        redraw = true;
    }

    function drawBackground() {
        for (var x = 0; x < BoardDim; x++)
            for (var y = 0; y < BoardDim; y++) {
                ctx.fillStyle = highlights.indexOf(indexAt(x, y)) != -1 ? "#afa" : (x + y) % 2 == 0 ? "#eee" : "#ddd";
                ctx.fillRect(x * cellSize, y * cellSize, cellSize + 1, cellSize + 1);
            }
    }

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

    function drawPiece(x, y, sign, selected) {
        x = (x + 0.5) * cellSize;
        y = (y + 0.5) * cellSize;

        if (animation != null && selected) {
            x = x * (1 - animation) + (getX(target) + 0.5) * cellSize * animation;
            y = y * (1 - animation) + (getY(target) + 0.5) * cellSize * animation;
        }

        // var radius = cellSize / 2 / 1.2;

        // var king = sign == "X" || sign == "O";
        // var color = sign == "x" || sign == "X" ? "#e33" : "#33e";

        // drawCircle(x, y, radius, 3, color, selected);

        // if (king && !selected) {
        //     drawCircle(x, y, radius / 1.3, 3, color);
        //     drawCircle(x, y, radius / 1.9, 3, color);
        //     drawCircle(x, y, radius / 3.5, 3, color);
        // }

        ctx.shadowOffsetX = 0;
        ctx.shadowOffsetY = 0;

        if (selected) {
            ctx.shadowColor = "#ff0";
            ctx.shadowBlur = 50;
        } else {
            ctx.shadowColor = "#000";
            ctx.shadowBlur = 10;
        }

        ctx.drawImage(images[sign], x - cellSize / 2, y - cellSize / 2, cellSize, cellSize);

        ctx.shadowColor = "transparent";
    }

    function processCell(i) {
        var sign = data[i];

        if (sign != " " && sign != "_")
            drawPiece(getX(i), getY(i), sign, selection == i);
    }

    function drawBoard() {
        if (data == null)
            return;

        for (var i = 0; i < data.length; i++)
            if (selection != i)
                processCell(i);

        if (selection != null)
            processCell(selection);

        if (animation != null) {
            animation += 0.05;

            if (animation >= 1) {
                if (getY(target) == 7 && data[selection] == "x")
                    data[target] = "X";
                else if (getY(target) == 0 && data[selection] == "o")
                    data[target] = "O";
                else
                    data[target] = data[selection];

                data[selection] = "_";

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
        ctx.rect(0, 0, boardSize, boardSize);
        ctx.closePath();
        ctx.stroke();
    }

    this.draw = function() {
        if (!redraw && animation == null)
            return;

        ctx.clearRect(0, 0, boardSize, boardSize);

        drawBackground();
        drawBoard();
        drawFrame();

        redraw = false;
    }
}
