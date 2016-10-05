var websocket = null;

$(document).ready(function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;

        if (websocket && websocket.readyState == 1)
            websocket.close();

        websocket = new WebSocket("ws://localhost:43567");

        websocket.onopen = function(evt) {};

        websocket.onclose = function(evt) {};

        websocket.onmessage = function(evt) {
            console.log("message: ", evt.data);
        };

        websocket.onerror = function(evt) {};
    } catch (exception) {
        console.log("error: ", exception);
    }
});

function sendMessage() {
    if (websocket != null) {
        websocket.send("message");
    }
}