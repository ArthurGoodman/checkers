#pragma once

#include <QObject>

class QWebSocketServer;
class QWebSocket;

class Engine;

class Server : public QObject {
    static const ushort port = 443;

    Engine *engine;

    QWebSocketServer *webSocketServer;
    QList<QWebSocket *> clients;

public:
    Server(Engine *engine);
    ~Server();

    void init(QWebSocket *client = 0);
    void select(int index);
    void deselect();
    void highlight(QWebSocket *client = 0);
    void move(int from, int to);
    void winner(const QString &winner);

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void socketDisconnected();

private:
    void sendMessage(const QString &message, QWebSocket *client = 0);
};
