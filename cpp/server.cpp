#include "server.h"

#include <QWebSocketServer>
#include <QWebSocket>

#include "engine.h"

Server::Server(Engine *engine)
    : engine(engine) {
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

    init(client);

    clients << client;
}

void Server::processMessage(const QString &message) {
    engine->click(message.toInt());
}

void Server::socketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}

void Server::init(QWebSocket *client) {
    const Board &board = engine->getBoard();

    sendMessage(QString("{\"cmd\": \"board\", \"data\": %1}").arg(board.toJson()), client);

    if (engine->getSelection() != -1)
        sendMessage(QString("{\"cmd\": \"select\", \"index\": %1}").arg(engine->getSelection()), client);

    highlight(client);
}

void Server::select(int index) {
    sendMessage(QString("{\"cmd\": \"select\", \"index\": %1}").arg(index));
}

void Server::deselect() {
    sendMessage("{\"cmd\": \"select\", \"index\": null}");
}

void Server::highlight(QWebSocket *client) {
    QString cells = "";

    QSet<int> highlights;

    const QVector<QPair<int, int>> &validMoves = engine->getValidMoves();

    for (int i = 0; i < validMoves.size(); i++) {
        int cell = engine->getSelection() == -1 ? validMoves.at(i).first : validMoves.at(i).second;

        if (!highlights.contains(cell)) {
            highlights << cell;
            cells += QString(i > 0 ? ", %1" : "%1").arg(cell);
        }
    }

    QString message = QString("{\"cmd\": \"highlight\", \"cells\": [%1]}").arg(cells);

    sendMessage(message, client);
}

void Server::move(int from, int to) {
    sendMessage(QString("{\"cmd\": \"move\", \"from\": %1, \"to\": %2}").arg(from).arg(to));

    int x0 = Board::getX(from), y0 = Board::getY(from);
    int x1 = Board::getX(to), y1 = Board::getY(to);

    if (qAbs(x0 - x1) == 2)
        sendMessage(QString("{\"cmd\": \"remove\", \"index\": %1}").arg(Board::indexAt((x0 + x1) / 2, (y0 + y1) / 2)));
}

void Server::winner(const QString &winner) {
    sendMessage(QString("{\"cmd\": \"winner\", \"winner\": \"%1\"}").arg(winner));
}

void Server::sendMessage(const QString &message, QWebSocket *client) {
    QTextStream(stdout) << message << "\n";

    if (client)
        client->sendTextMessage(message);
    else
        for (QWebSocket *client : clients)
            client->sendTextMessage(message);
}
