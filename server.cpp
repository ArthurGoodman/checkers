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
            client->sendTextMessage(message);

            if (qAbs(Board::getX(json["from"].toInt()) - Board::getX(json["to"].toInt())) == 2)
                client->sendTextMessage("{\"cmd\":\"remove\",\"location\":" + QString::number(Board::indexAt((Board::getX(json["from"].toInt()) + Board::getX(json["to"].toInt())) / 2, (Board::getY(json["from"].toInt()) + Board::getY(json["to"].toInt())) / 2)) + "}");

            if (pl.checkPlayerWon()) {
                client->sendTextMessage("{\"cmd\":\"winner\",\"winner\":\"o\"}");
                pl.reset();
            } else {
                Board board = pl.getBoard();

                pl.ai();

                QVector<QPair<int, int>> moves = parseMoves(board, pl.getBoard());

                for (const QPair<int, int> &move : moves) {
                    client->sendTextMessage("{\"cmd\":\"move\",\"from\":" + QString::number(move.first) + ",\"to\":" + QString::number(move.second) + "}");

                    if (qAbs(Board::getX(move.first) - Board::getX(move.second)) == 2)
                        client->sendTextMessage("{\"cmd\":\"remove\",\"location\":" + QString::number(Board::indexAt((Board::getX(move.first) + Board::getX(move.second)) / 2, (Board::getY(move.first) + Board::getY(move.second)) / 2)) + "}");
                }

                if (pl.checkAiWon()) {
                    client->sendTextMessage("{\"cmd\":\"winner\",\"winner\":\"x\"}");
                    pl.reset();
                }
            }
        }
    }
}

void Server::socketDisconnected() {
    delete client;
    client = 0;
}

QVector<QPair<int, int>> Server::parseMoves(const Board &a, const Board &b) {
    QVector<QPair<int, int>> moves;

    int location = -1, newLocation = -1;
    int x, y;

    for (int i = 0; i < Board::size(); i++)
        if ((a.at(i) == 'x' || a.at(i) == 'y') && b.at(i) == 'e') {
            location = i;
            x = Board::getX(location);
            y = Board::getY(location);
            break;
        }

    char type = a.at(location);

    QSet<QPair<int, int>> eaten;

    while (location != -1) {
        if (a.at(x + 1, y + 1) == 'e' && (b.at(x + 1, y + 1) == 'x' || b.at(x + 1, y + 1) == 'y'))
            moves << qMakePair(location, newLocation = Board::indexAt(x + 1, y + 1));
        else if (a.at(x - 1, y + 1) == 'e' && (b.at(x - 1, y + 1) == 'x' || b.at(x - 1, y + 1) == 'y'))
            moves << qMakePair(location, newLocation = Board::indexAt(x - 1, y + 1));
        else if ((a.at(x + 1, y + 1) == 'o' || a.at(x + 1, y + 1) == 'p') && b.at(x + 1, y + 1) == 'e' && !eaten.contains(qMakePair(x + 1, y + 1))) {
            moves << qMakePair(location, newLocation = Board::indexAt(x + 2, y + 2));
            eaten.insert(qMakePair(x + 1, y + 1));
        } else if ((a.at(x - 1, y + 1) == 'o' || a.at(x - 1, y + 1) == 'p') && b.at(x - 1, y + 1) == 'e' && !eaten.contains(qMakePair(x - 1, y + 1))) {
            moves << qMakePair(location, newLocation = Board::indexAt(x - 2, y + 2));
            eaten.insert(qMakePair(x - 1, y + 1));
        } else if (type == 'y') {
            if (a.at(x + 1, y - 1) == 'e' && b.at(x + 1, y - 1) == 'y')
                moves << qMakePair(location, newLocation = Board::indexAt(x + 1, y - 1));
            else if (a.at(x - 1, y - 1) == 'e' && b.at(x - 1, y - 1) == 'y')
                moves << qMakePair(location, newLocation = Board::indexAt(x - 1, y - 1));
            else if ((a.at(x + 1, y - 1) == 'o' || a.at(x + 1, y - 1) == 'p') && b.at(x + 1, y - 1) == 'e' && !eaten.contains(qMakePair(x + 1, y - 1))) {
                moves << qMakePair(location, newLocation = Board::indexAt(x + 2, y - 2));
                eaten.insert(qMakePair(x + 1, y - 1));
            } else if ((a.at(x - 1, y - 1) == 'o' || a.at(x - 1, y - 1) == 'p') && b.at(x - 1, y - 1) == 'e' && !eaten.contains(qMakePair(x - 1, y - 1))) {
                moves << qMakePair(location, newLocation = Board::indexAt(x - 2, y - 2));
                eaten.insert(qMakePair(x - 1, y - 1));
            }
        }

        location = newLocation;
        x = Board::getX(location);
        y = Board::getY(location);

        newLocation = -1;
    }

    return moves;
}
