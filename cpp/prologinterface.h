#pragma once

#include "board.h"

class PrologInterface {
    PlEngine engine;
    Board board;
    int depth;

public:
    PrologInterface(int depth);

    bool move(int from, int to);
    void ai();
    bool checkForWinner(const QString &s);
    const Board &getBoard() const;

    void reset();
};
