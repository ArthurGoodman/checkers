#include "prologinterface.h"

#include <exception>

static char *args[] = {const_cast<char *>("-t"), const_cast<char *>("--quiet"), const_cast<char *>("--nosignals")};

PrologInterface::PrologInterface(int depth)
    : engine(sizeof(args) / sizeof(char *), args), depth(depth) {
    try {
        PlCall("consult('pl/alphabeta.pl').");
        PlCall("consult('pl/game.pl').");
        PlCall("consult('pl/heuristic.pl').");
        PlCall("consult('pl/moves.pl').");
        PlCall("consult('pl/utility.pl').");

        reset();
    } catch (PlException &e) {
        QTextStream(stdout) << (char *)e << "\n";
    }
}

bool PrologInterface::move(int from, int to) {
    try {
        PlTermv av(6);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = Board::getX(from) + 1;
        av[2] = Board::getY(from) + 1;
        av[3] = Board::getX(to) + 1;
        av[4] = Board::getY(to) + 1;

        if (!PlCall("move", av))
            return false;

        board = av[5];
    } catch (PlException &e) {
        QTextStream(stdout) << (char *)e << "\n";
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
        av[5] = depth;

        PlCall("alphabeta", av);

        board = av[3][2];
    } catch (PlException &e) {
        QTextStream(stdout) << (char *)e << "\n";
    }
}

bool PrologInterface::checkForWinner(const QString &s) {
    try {
        PlTermv av(2);
        PlCall("term_to_atom", PlTermv(av[0], board.toTerm().toStdString().data()));
        av[1] = s.toStdString().data();

        return PlCall("goal", av);
    } catch (PlException &e) {
        QTextStream(stdout) << (char *)e << "\n";
        return false;
    }
}

const Board &PrologInterface::getBoard() const {
    return board;
}

void PrologInterface::reset() {
    PlTermv av(1);
    PlCall("init", av);
    board = av[0];
}
