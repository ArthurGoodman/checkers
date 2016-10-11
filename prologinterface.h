#pragma once

#include "board.h"

class PrologInterface {
    PlEngine engine;
    Board board;

public:
    PrologInterface();

    bool move(int from, int to);
    void ai();
    bool checkPlayerWon();
    bool checkAiWon();
    const Board &getBoard();

    void reset();
};
