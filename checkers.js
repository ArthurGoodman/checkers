var websocket = null;
var canvas = $("#checkers")[0];
var DrawContext = canvas.getContext('2d');

$(document).ready(function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;

        if (websocket && websocket.readyState == 1)
            websocket.close();

        websocket = new WebSocket('ws://localhost:43567');

        websocket.onopen = function(evt) {};

        websocket.onclose = function(evt) {};

        websocket.onmessage = function(evt) {
            console.log('message: ', evt.data);

            DrawContext.clearRect(0, 0, canvas.width, canvas.height);
            DrawContext.fillStyle = evt.data;
            DrawContext.fillRect(0, 0, canvas.width, canvas.height);

            if (websocket != null) {
                var obj = {
                    field: "value"
                };

                websocket.send(JSON.stringify(obj));
            }
        };

        websocket.onerror = function(evt) {};
    } catch (exception) {
        console.log('error: ', exception);
    }
})