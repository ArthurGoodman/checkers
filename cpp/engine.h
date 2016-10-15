#pragma once

#include "server.h"
#include "prologinterface.h"

class Engine {
    Server server;
    PrologInterface pl;

    int selection;
    QVector<QPair<int, int>> validMoves;
    bool lastMoveIsEat, lockSelection;

public:
    Engine(int depth);

    void click(int index);
    const Board &getBoard() const;
    int getSelection() const;
    const QVector<QPair<int, int>> &getValidMoves() const;

private:
    void reset();
    void select(int index);
    void deselect();
    void findValidMoves();
    QVector<QPair<int, int>> parseMoves(const Board &a, const Board &b);
    bool isMoveValid(int from, int to) const;
};
