#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;

class Server : public QObject {
    static const ushort port = 43567;

    QWebSocketServer *webSocketServer;
    QList<QWebSocket *> clients;

public:
    explicit Server();
    ~Server();

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();
};
