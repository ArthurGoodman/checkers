#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;

#include "prologinterface.h"

class Server : public QObject {
    static const ushort port = 43567;

    QWebSocketServer *webSocketServer;
    QWebSocket *client;

    PrologInterface pl;

public:
    explicit Server();
    ~Server();

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    QVector<QPair<int, int>> parseMoves(const Board &a, const Board &b);
};
