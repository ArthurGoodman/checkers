#pragma once

#include <QString>
#include <QVector>

class Board {
public:
    static const int BoardWidth = 8;
    static const int BoardHeight = 8;

private:
    QVector<char> data;

public:
    Board();
    Board(const QVector<char> &data);

    QString toJson() const;
};
