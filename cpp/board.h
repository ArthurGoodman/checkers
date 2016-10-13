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
    static int getX(int index);
    static int getY(int index);
    static int indexAt(int x, int y);
    static int size();

    Board();
    Board(const Board &b);
    Board(const PlTerm &term);

    Board &operator=(const Board &b);
    Board &operator=(const PlTerm &term);

    QString toTerm() const;
    QString toJson() const;

    char at(int i) const;
    char at(int x, int y) const;
};
