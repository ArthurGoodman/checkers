#include "server.h"

#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

#include "board.h"

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
    QWebSocket *client = webSocketServer->nextPendingConnection();

    connect(client, &QWebSocket::textMessageReceived, this, &Server::processMessage);
    connect(client, &QWebSocket::disconnected, this, &Server::socketDisconnected);

    client->sendTextMessage(QString("{\"cmd\": \"board\", \"board\": %1}").arg(pl.getBoard().toJson()));

    clients << client;
}

void Server::processMessage(const QString &message) {
    QTextStream(stdout) << message;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject json = jsonDoc.object();

    if (json["cmd"] == "move") {
        int from = json["from"].toInt(), to = json["to"].toInt();

        if (pl.move(from, to)) {
            move(from, to);

            if (pl.checkPlayerWon()) {
                winner("o");
                pl.reset();
            } else {
                Board board = pl.getBoard();

                pl.ai();

                QVector<QPair<int, int>> moves = parseMoves(board, pl.getBoard());

                for (const QPair<int, int> &m : moves)
                    move(m.first, m.second);

                if (pl.checkAiWon()) {
                    winner("x");
                    pl.reset();
                }
            }
        }
    }
}

void Server::socketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}

void Server::winner(const QString &winner) {
    sendMessage(QString("{\"cmd\": \"winner\", \"winner\": \"%1\"}").arg(winner));
}

void Server::move(int from, int to) {
    sendMessage(QString("{\"cmd\": \"move\", \"from\": %1, \"to\": %2}").arg(from).arg(to));

    int x0 = Board::getX(from), y0 = Board::getY(from);
    int x1 = Board::getX(to), y1 = Board::getY(to);

    if (qAbs(x0 - x1) == 2)
        sendMessage(QString("{\"cmd\": \"remove\", \"location\": %1}").arg(Board::indexAt((x0 + x1) / 2, (y0 + y1) / 2)));
}

void Server::sendMessage(const QString &message) {
    for (QWebSocket *client : clients)
        client->sendTextMessage(message);
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
