#include "server.h"

#include <QWebSocketServer>
#include <QWebSocket>

#include "board.h"

Server::Server()
    : selection(-1) {
    findValidMoves();

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
    const Board &board = pl.getBoard();

    int index = message.toInt(), x = Board::getX(index), y = Board::getY(index);

    if (selection != index && (board.at(x, y) == 'o' || board.at(x, y) == 'p'))
        select(index);
    else if (selection == index || (selection != -1 && board.at(x, y) != 'e'))
        deselect();
    else if (selection != -1) {
        if (isMoveValid(selection, index)) {
            pl.move(selection, index);

            lastMoveIsEat = false;

            if (qAbs(Board::getX(selection) - x) == 2)
                lastMoveIsEat = true;

            move(selection, index);

            selection = index;
            lockSelection = false;
            findValidMoves();

            if (pl.checkPlayerWon()) {
                winner("o");
                reset();
                return;
            } else if (!lockSelection) {
                selection = -1;

                Board board = pl.getBoard();

                pl.ai();

                QVector<QPair<int, int>> moves = parseMoves(board, pl.getBoard());

                for (const QPair<int, int> &m : moves)
                    move(m.first, m.second);

                if (pl.checkAiWon()) {
                    winner("x");
                    reset();
                    return;
                }
            } else {
                select(selection);
                highlight();
            }
        } else
            deselect();
    } else
        return;

    if (!lockSelection) {
        findValidMoves();
        highlight();
    }
}

void Server::socketDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        clients.removeAll(client);
        client->deleteLater();
    }
}

void Server::reset() {
    pl.reset();
    init();
}

void Server::init(QWebSocket *client) {
    Board board = pl.getBoard();

    sendMessage(QString("{\"cmd\": \"board\", \"board\": %1}").arg(board.toJson()), client);

    if (selection != -1)
        sendMessage(QString("{\"cmd\": \"select\", \"index\": %1}").arg(selection), client);

    findValidMoves();
    highlight(client);
}

void Server::select(int index) {
    if (selection != index && lockSelection)
        return;

    selection = index;
    sendMessage(QString("{\"cmd\": \"select\", \"index\": %1}").arg(index));
}

void Server::deselect() {
    if (lockSelection)
        return;

    selection = -1;
    sendMessage("{\"cmd\": \"deselect\"}");
}

void Server::highlight(QWebSocket *client) {
    QString moves = "";

    for (int i = 0; i < validMoves.size(); i++)
        moves += QString(i > 0 ? ", %1" : "%1").arg(selection == -1 ? validMoves[i].first : validMoves[i].second);

    QString message = QString("{\"cmd\": \"highlight\", \"cells\": [%1]}").arg(moves);

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

bool Server::isMoveValid(int from, int to) {
    return validMoves.contains(qMakePair(from, to));
}

void Server::findValidMoves() {
    const Board &board = pl.getBoard();

    validMoves.clear();

    for (int x = 0; x < Board::BoardDim; x++)
        for (int y = 0; y < Board::BoardDim; y++) {
            int index = Board::indexAt(x, y);

            if (board.at(index) == 'o' || board.at(index) == 'p') {
                if ((board.at(x + 1, y - 1) == 'x' || board.at(x + 1, y - 1) == 'y') && board.at(x + 2, y - 2) == 'e')
                    validMoves << qMakePair(index, Board::indexAt(x + 2, y - 2));
                if ((board.at(x - 1, y - 1) == 'x' || board.at(x - 1, y - 1) == 'y') && board.at(x - 2, y - 2) == 'e')
                    validMoves << qMakePair(index, Board::indexAt(x - 2, y - 2));

                if (board.at(index) == 'p') {
                    if ((board.at(x + 1, y + 1) == 'x' || board.at(x + 1, y + 1) == 'y') && board.at(x + 2, y + 2) == 'e')
                        validMoves << qMakePair(index, Board::indexAt(x + 2, y + 2));
                    if ((board.at(x - 1, y + 1) == 'x' || board.at(x - 1, y + 1) == 'y') && board.at(x - 2, y + 2) == 'e')
                        validMoves << qMakePair(index, Board::indexAt(x - 2, y + 2));
                }
            }
        }

    if (validMoves.empty()) {
        for (int x = 0; x < Board::BoardDim; x++)
            for (int y = 0; y < Board::BoardDim; y++) {
                int index = Board::indexAt(x, y);

                if (selection != -1 && selection != index)
                    continue;

                if (board.at(index) == 'o' || board.at(index) == 'p') {
                    if (board.at(x + 1, y - 1) == 'e')
                        validMoves << qMakePair(index, Board::indexAt(x + 1, y - 1));
                    if (board.at(x - 1, y - 1) == 'e')
                        validMoves << qMakePair(index, Board::indexAt(x - 1, y - 1));

                    if (board.at(index) == 'p') {
                        if (board.at(x + 1, y + 1) == 'e')
                            validMoves << qMakePair(index, Board::indexAt(x + 1, y + 1));
                        if (board.at(x - 1, y + 1) == 'e')
                            validMoves << qMakePair(index, Board::indexAt(x - 1, y + 1));
                    }
                }
            }
    } else if (selection != -1) {
        if (lastMoveIsEat)
            lockSelection = true;

        bool clear = true;

        for (int i = 0; i < validMoves.size(); i++)
            if (validMoves[i].first == selection)
                clear = false;

        if (clear)
            validMoves.clear();
        else {
            QMutableVectorIterator<QPair<int, int>> i(validMoves);

            while (i.hasNext()) {
                if (i.next().first != selection)
                    i.remove();
            }
        }
    }
}
