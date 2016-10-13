"use strict";

var canvas = $("#checkers")[0];

var board = new Board(canvas);
var engine = new Engine(board);

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

    engine.click(x, y);
});

$(window).on("resize", function() {
    board.resize(Math.min(window.innerWidth, window.innerHeight) - 70);
});

$(window).resize();

$("#reset").on("click", function() {
    engine.reset();
});

$(document).ready(function() {
    engine.start();

    function tick() {
        engine.tick();
        window.requestAnimationFrame(tick);
    }

    tick();
})