#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;

#include "prologinterface.h"

class Server : public QObject {
    static const ushort port = 443;

    QWebSocketServer *webSocketServer;
    QList<QWebSocket *> clients;

    PrologInterface pl;

    int selection;
    QVector<QPair<int, int>> validMoves;
    bool lastMoveIsEat, lockSelection;

public:
    explicit Server();
    ~Server();

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    void reset();
    void init(QWebSocket *client = 0);
    void select(int index);
    void deselect();
    void highlight(QWebSocket *client = 0);
    void move(int from, int to);
    void winner(const QString &winner);
    void sendMessage(const QString &message, QWebSocket *client = 0);
    QVector<QPair<int, int>> parseMoves(const Board &a, const Board &b);
    bool isMoveValid(int from, int to);
    void findValidMoves();
};
