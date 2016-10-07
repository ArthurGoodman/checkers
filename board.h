#pragma once

#include <QString>
#include <QVector>

class Board {
    QVector<char> data;

public:
    Board();
    Board(const QVector<char> &data);

    QString toJson() const;
};
