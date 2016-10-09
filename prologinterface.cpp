#include "prologinterface.h"

#include <iostream>
#include <exception>

#include "board.h"

static char *args[] = {const_cast<char *>("-t"), const_cast<char *>("--quiet"), const_cast<char *>("--nosignals")};

PrologInterface::PrologInterface()
    : engine(sizeof(args) / sizeof(char *), args) {
    try {
        PlCall("consult('checkers.pl').");

        PlCall("assert(min_to_move(o/_)).");
        PlCall("assert(max_to_move(x/_)).");

        PlTermv av(1);
        PlCall("init", av);

        board = av[0];
    } catch (const PlException &e) {
        std::cout << (char *)e;
    }
}

bool PrologInterface::move(int from, int to) {
    try {
        long fromL = from / Board::BoardDim + 1;
        long fromC = from % Board::BoardDim + 1;
        long toL = to / Board::BoardDim + 1;
        long toC = to % Board::BoardDim + 1;

        PlTermv av(6);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = fromL;
        av[2] = fromC;
        av[3] = toL;
        av[4] = toC;

        if (!PlCall("move", av))
            return false;

        board = av[5];
    } catch (const PlException &e) {
        std::cout << (char *)e;
        return false;
    }

    return true;
}

void PrologInterface::ai() {
    try {
        PlTermv av(6);
        PlCall("term_to_atom", PlTermv(av[0], ("x/" + board.toTerm()).toStdString().data()));
        av[1] = -100;
        av[2] = 100;
        av[5] = 2;

        PlCall("alphabeta", av);

        board = av[3][2];
    } catch (const PlException &e) {
        std::cout << (char *)e;
    }
}

bool PrologInterface::checkPlayerWon() {
    try {
        PlTermv av(2);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = "o";

        return PlCall("goal", av);
    } catch (const PlException &e) {
        std::cout << (char *)e;
        return false;
    }
}

bool PrologInterface::checkAiWon() {
    try {
        PlTermv av(2);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = "x";

        return PlCall("goal", av);
    } catch (const PlException &e) {
        std::cout << (char *)e;
        return false;
    }
}

Board PrologInterface::getBoard() {
    return board;
}
