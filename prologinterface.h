#pragma once

#include <QVector>

#include <SWI-cpp.h>

class Board;

class PrologInterface {
    PlEngine engine;

public:
    PrologInterface();

    Board board();
};
