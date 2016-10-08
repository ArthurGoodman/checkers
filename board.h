#pragma once

#include <QString>
#include <QVector>

class Board {
public:
    static const int BoardDim = 8;

private:
    QVector<char> data;

public:
    Board();
    Board(const QVector<char> &data);

    QString toJson() const;
};
