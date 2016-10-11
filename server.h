#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;

#include "prologinterface.h"

class Server : public QObject {
    static const ushort port = 43567;

    QWebSocketServer *webSocketServer;
    QList<QWebSocket *> clients;

    PrologInterface pl;

    int selection;
    QVector<QPair<int, int>> validMoves;

public:
    explicit Server();
    ~Server();

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    void init(QWebSocket *client);
    void select(int index);
    void deselect();
    void highlight();
    void move(int from, int to);
    void winner(const QString &winner);
    void sendMessage(const QString &message);
    QVector<QPair<int, int>> parseMoves(const Board &a, const Board &b);
    void findValidMoves(const Board &board);
};
