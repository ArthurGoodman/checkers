#include "prologinterface.h"

#include <iostream>
#include <exception>

#include "board.h"

static char *args[2] = {"-t", "-q"};

PrologInterface::PrologInterface()
    : engine(2, args) {
    try {
        PlCall("consult('checkers.pl').");
    } catch (PlException &ex) {
        std::cout << (char *)ex;
    }
}

Board PrologInterface::board() {
    QVector<char> board;

    try {
        PlTerm term;
        PlCall("board", PlTermv(term));

        if (term.arity() != 64)
            throw std::runtime_error("invalid board");

        for (int i = 1; i <= term.arity(); i++)
            board << ((char *)term[i])[0];

        return Board(board);
    } catch (PlException &ex) {
        std::cout << (char *)ex;
    } catch (const std::exception &e) {
        std::cout << "error: " << e.what() << "\n";
    }

    return Board();
}
