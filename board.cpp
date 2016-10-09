#include "board.h"

#include <iostream>
#include <exception>

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

        for (int i = 1; i <= term.arity(); i++) {
            std::string str = (char *)term[i];

            if (str == "xx")
                str = "y";
            else if (str == "oo")
                str = "p";

            data << str[0];
        }
    } catch (const PlException &e) {
        std::cout << (char *)e;
    } catch (const std::exception &e) {
        std::cout << "error: " << e.what() << "\n";
    }

    if (this->data.size() != BoardDim * BoardDim)
        this->data.clear();

    return *this;
}

QString Board::toTerm() const {
    if (data.isEmpty())
        return "";

    QString term = "b(";

    for (int i = 0; i < data.size(); i++) {
        if (i > 0)
            term += ",";

        QString str;

        if (data[i] == 'y')
            str = "xx";
        else if (data[i] == 'p')
            str = "oo";
        else
            str = data[i];

        term += str;
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

        json += (QString) "\"" + data[i] + "\"";
    }

    return json + "]";
}
