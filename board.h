#pragma once

#include <QString>
#include <QVector>

#include <SWI-cpp.h>

class Board {
public:
    static const int BoardDim = 8;

private:
    QVector<char> data;

public:
    Board();
    Board(const Board &b);
    Board(const PlTerm &term);

    Board &operator=(const Board &b);
    Board &operator=(const PlTerm &term);

    QString toTerm() const;
    QString toJson() const;
};
