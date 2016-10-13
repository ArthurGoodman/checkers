#pragma once

#include "board.h"

class PrologInterface {
    PlEngine engine;
    Board board;

public:
    PrologInterface();

    bool move(int from, int to);
    void ai();
    bool checkForWinner(const QString &s);
    const Board &getBoard();

    void reset();
};
