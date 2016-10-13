#include "prologinterface.h"

#include <iostream>
#include <exception>

#include "board.h"

static char *args[] = {const_cast<char *>("-t"), const_cast<char *>("--quiet"), const_cast<char *>("--nosignals")};

PrologInterface::PrologInterface()
    : engine(sizeof(args) / sizeof(char *), args) {
    try {
        PlCall("consult('pl/checkers.pl').");

        PlCall("assert(min_to_move(o/_)).");
        PlCall("assert(max_to_move(x/_)).");

        reset();
    } catch (const PlException &e) {
        std::cout << (char *)e;
    }
}

bool PrologInterface::move(int from, int to) {
    try {
        PlTermv av(6);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = Board::getY(from) + 1;
        av[2] = Board::getX(from) + 1;
        av[3] = Board::getY(to) + 1;
        av[4] = Board::getX(to) + 1;

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

bool PrologInterface::checkForWinner(const QString &s) {
    try {
        PlTermv av(2);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = s.toStdString().data();

        return PlCall("goal", av);
    } catch (const PlException &e) {
        std::cout << (char *)e;
        return false;
    }
}

const Board &PrologInterface::getBoard() {
    return board;
}

void PrologInterface::reset() {
    PlTermv av(1);
    PlCall("init", av);
    board = av[0];
}
