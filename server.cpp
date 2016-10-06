#include "server.h"

#include <QWebSocketServer>
#include <QWebSocket>

Server::Server() {
    webSocketServer = new QWebSocketServer("Checkers Server", QWebSocketServer::NonSecureMode, this);

    if (webSocketServer->listen(QHostAddress::Any, port))
        connect(webSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    webSocketServer->close();
    qDeleteAll(clients.begin(), clients.end());
}

void Server::onNewConnection() {
    QWebSocket *socket = webSocketServer->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &Server::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

    clients << socket;

    socket->sendTextMessage("#ff0000");
}

void Server::processMessage(const QString &message) {
    qDebug() << message;
}

void Server::socketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}
