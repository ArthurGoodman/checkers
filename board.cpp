#include "board.h"

Board::Board() {
}

Board::Board(const QVector<char> &data)
    : data(data) {
    if (this->data.size() != BoardWidth * BoardHeight)
        this->data.clear();
}

QString Board::toJson() const {
    QString json = "[";

    for (int i = 0; i < data.size(); i++) {
        if (i > 0)
            json += ",";

        json += (QString) "\"" + data[i] + "\"";
    }

    return json + "]";
}
