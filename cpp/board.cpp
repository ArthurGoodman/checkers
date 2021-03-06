#include "board.h"

#include <exception>

int Board::getX(int index) {
    return index % BoardDim;
}

int Board::getY(int index) {
    return index / BoardDim;
}

int Board::indexAt(int x, int y) {
    return y * BoardDim + x;
}

int Board::size() {
    return BoardDim * BoardDim;
}

Board::Board() {
}

Board::Board(const Board &b) {
    *this = b;
}

Board::Board(const PlTerm &term) {
    *this = term;
}

Board &Board::operator=(const Board &b) {
    data = b.data;
    return *this;
}

Board &Board::operator=(const PlTerm &term) {
    data.clear();

    try {
        if (term.arity() != Board::BoardDim * Board::BoardDim)
            throw std::runtime_error("invalid board");

        for (int i = 1; i <= term.arity(); i++)
            data << ((char *)term[i])[0];
    } catch (PlException &e) {
        QTextStream(stdout) << (char *)e << "\n";
    } catch (const std::exception &e) {
        QTextStream(stdout) << "error: " << e.what() << "\n";
    }

    if (this->data.size() != BoardDim * BoardDim)
        this->data.clear();

    return *this;
}

QString Board::toTerm() const {
    if (data.isEmpty())
        return "";

    QString term = "board(";

    for (int i = 0; i < data.size(); i++) {
        if (i > 0)
            term += ",";

        term += QString("'%1'").arg(data[i]);
    }

    return term + ")";
}

QString Board::toJson() const {
    if (data.isEmpty())
        return "null";

    QString json = "[";

    for (int i = 0; i < data.size(); i++) {
        if (i > 0)
            json += ",";

        json += QString("\"%1\"").arg(data[i]);
    }

    return json + "]";
}

char Board::at(int i) const {
    if (i < 0 || i >= size())
        return ' ';

    return data.at(i);
}

char Board::at(int x, int y) const {
    if (x < 0 || x >= BoardDim || y < 0 || y >= BoardDim)
        return ' ';

    return data.at(indexAt(x, y));
}
