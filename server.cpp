#include "server.h"

#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include "board.h"

Server::Server()
    : client(0) {
    webSocketServer = new QWebSocketServer("Checkers Server", QWebSocketServer::NonSecureMode, this);

    if (webSocketServer->listen(QHostAddress::Any, port))
        connect(webSocketServer, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
}

Server::~Server() {
    webSocketServer->close();
    delete client;
}

void Server::onNewConnection() {
    if (client == 0) {
        client = webSocketServer->nextPendingConnection();

        connect(client, &QWebSocket::textMessageReceived, this, &Server::processMessage);
        connect(client, &QWebSocket::disconnected, this, &Server::socketDisconnected);

        client->sendTextMessage("{\"cmd\":\"board\",\"board\":" + pl.getBoard().toJson() + "}");
    }
}

void Server::processMessage(const QString &message) {
    qDebug().noquote() << message;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject json = jsonDoc.object();

    if (json["cmd"] == "move") {
        if (pl.move(json["from"].toInt(), json["to"].toInt())) {
            if (pl.checkPlayerWon())
                client->sendTextMessage("{\"cmd\":\"winner\",\"winner\":\"o\"}");
            else {
                pl.ai();

                if (pl.checkAiWon())
                    client->sendTextMessage("{\"cmd\":\"winner\",\"winner\":\"x\"}");
            }

            client->sendTextMessage("{\"cmd\":\"board\",\"board\":" + pl.getBoard().toJson() + "}");
        }
    }
}

void Server::socketDisconnected() {
    delete client;
    client = 0;
}
