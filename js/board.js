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
        return data[indexAt(x, y)];
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
        redraw = true;
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

    this.deselect = function() {
        selection = null;
        redraw = true;
    }

    this.remove = function(index) {
        data[index] = "e";
        redraw = true;
    }

    this.highlight = function(cells) {
        highlights = cells;
        redraw = true;
    }

    function drawBackground() {
        ctx.clearRect(0, 0, boardSize, boardSize);

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

    function drawPiece(x, y, color, king) {
        var fill = selection == indexAt(x, y);

        x = (x + 0.5) * cellSize;
        y = (y + 0.5) * cellSize;

        if (animation != null && fill) {
            x = x * (1 - animation) + (getX(target) + 0.5) * cellSize * animation;
            y = y * (1 - animation) + (getY(target) + 0.5) * cellSize * animation;
        }

        var radius = cellSize / 2 / 1.2;

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

    function drawBoard() {
        if (data == null)
            return;

        for (var x = 0; x < BoardDim; x++)
            for (var y = 0; y < BoardDim; y++)
                if (selection != indexAt(x, y))
                    processCell(x, y);

        if (selection != null)
            processCell(getX(selection), getY(selection));

        if (animation != null) {
            animation += 0.05;

            if (animation >= 1) {
                if (getY(target) == 7 && data[selection] == "x")
                    data[target] = "y";
                else if (getY(target) == 0 && data[selection] == "o")
                    data[target] = "p";
                else
                    data[target] = data[selection];

                data[selection] = "e";

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

        drawBackground();
        drawBoard();
        drawFrame();

        redraw = false;
    }
}