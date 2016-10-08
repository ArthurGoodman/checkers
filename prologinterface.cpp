#include "prologinterface.h"

#include <iostream>
#include <exception>

#include "board.h"

static char *args[] = {const_cast<char *>("-t"), const_cast<char *>("--quiet"), const_cast<char *>("--nosignals")};

PrologInterface::PrologInterface()
    : engine(sizeof(args) / sizeof(char *), args) {
    try {
        PlCall("consult('checkers.pl').");
    } catch (const PlException &e) {
        std::cout << (char *)e;
    }
}

Board PrologInterface::board() {
    QVector<char> board;

    try {
        PlTerm term;
        PlCall("board", PlTermv(term));

        if (term.arity() != Board::BoardWidth * Board::BoardHeight)
            throw std::runtime_error("invalid board");

        for (int i = 1; i <= term.arity(); i++)
            board << ((char *)term[i])[0];

        return Board(board);
    } catch (const PlException &e) {
        std::cout << (char *)e;
    } catch (const std::exception &e) {
        std::cout << "error: " << e.what() << "\n";
    }

    return Board();
}
