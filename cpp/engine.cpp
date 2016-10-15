#include "engine.h"

#include <QSet>

#include "server.h"

Engine::Engine(int depth)
    : server(this), pl(depth), selection(-1) {
    findValidMoves();
}

void Engine::click(int index) {
    const Board &board = pl.getBoard();

    int x = Board::getX(index), y = Board::getY(index);

    if (index < 0) {
        reset();
        return;
    }

    if (selection != index && (board.at(x, y) == 'o' || board.at(x, y) == 'O'))
        select(index);
    else if (selection == index || (selection != -1 && board.at(x, y) != '_'))
        deselect();
    else if (selection != -1) {
        if (isMoveValid(selection, index)) {
            pl.move(selection, index);
            server.move(selection, index);

            lastMoveIsEat = false;

            if (qAbs(Board::getX(selection) - x) == 2)
                lastMoveIsEat = true;

            selection = index;
            lockSelection = false;
            findValidMoves();

            lastMoveIsEat = false;

            if (pl.checkForWinner("o")) {
                if (pl.checkForWinner("x"))
                    server.winner("");
                else
                    server.winner("o");

                reset();
                return;
            } else if (!lockSelection) {
                selection = -1;

                Board board = pl.getBoard();

                pl.ai();

                QVector<QPair<int, int>> moves = parseMoves(board, pl.getBoard());

                for (const QPair<int, int> &m : moves)
                    server.move(m.first, m.second);

                if (pl.checkForWinner("x")) {
                    if (pl.checkForWinner("o"))
                        server.winner("");
                    else
                        server.winner("x");

                    reset();
                    return;
                }
            } else {
                select(selection);
                server.highlight();
            }
        } else
            deselect();
    } else
        return;

    if (!lockSelection) {
        findValidMoves();
        server.highlight();
    }
}

const QVector<QPair<int, int>> &Engine::getValidMoves() const {
    return validMoves;
}

const Board &Engine::getBoard() const {
    return pl.getBoard();
}

int Engine::getSelection() const {
    return selection;
}

void Engine::reset() {
    selection = -1;
    lastMoveIsEat = false;
    lockSelection = false;

    pl.reset();

    findValidMoves();

    server.init();
}

void Engine::select(int index) {
    if (selection != index && lockSelection)
        return;

    server.select(selection = index);
}

void Engine::deselect() {
    if (lockSelection)
        return;

    selection = -1;
    server.deselect();
}

void Engine::findValidMoves() {
    const Board &board = pl.getBoard();

    validMoves.clear();

    for (int x = 0; x < Board::BoardDim; x++)
        for (int y = 0; y < Board::BoardDim; y++) {
            int index = Board::indexAt(x, y);

            if (board.at(index) == 'o' || board.at(index) == 'O') {
                if ((board.at(x + 1, y - 1) == 'x' || board.at(x + 1, y - 1) == 'X') && board.at(x + 2, y - 2) == '_')
                    validMoves << qMakePair(index, Board::indexAt(x + 2, y - 2));
                if ((board.at(x - 1, y - 1) == 'x' || board.at(x - 1, y - 1) == 'X') && board.at(x - 2, y - 2) == '_')
                    validMoves << qMakePair(index, Board::indexAt(x - 2, y - 2));

                if (board.at(index) == 'O') {
                    if ((board.at(x + 1, y + 1) == 'x' || board.at(x + 1, y + 1) == 'X') && board.at(x + 2, y + 2) == '_')
                        validMoves << qMakePair(index, Board::indexAt(x + 2, y + 2));
                    if ((board.at(x - 1, y + 1) == 'x' || board.at(x - 1, y + 1) == 'X') && board.at(x - 2, y + 2) == '_')
                        validMoves << qMakePair(index, Board::indexAt(x - 2, y + 2));
                }
            }
        }

    if (validMoves.empty()) {
        for (int x = 0; x < Board::BoardDim; x++)
            for (int y = 0; y < Board::BoardDim; y++) {
                int index = Board::indexAt(x, y);

                if (selection != -1 && selection != index)
                    continue;

                if (board.at(index) == 'o' || board.at(index) == 'O') {
                    if (board.at(x + 1, y - 1) == '_')
                        validMoves << qMakePair(index, Board::indexAt(x + 1, y - 1));
                    if (board.at(x - 1, y - 1) == '_')
                        validMoves << qMakePair(index, Board::indexAt(x - 1, y - 1));

                    if (board.at(index) == 'O') {
                        if (board.at(x + 1, y + 1) == '_')
                            validMoves << qMakePair(index, Board::indexAt(x + 1, y + 1));
                        if (board.at(x - 1, y + 1) == '_')
                            validMoves << qMakePair(index, Board::indexAt(x - 1, y + 1));
                    }
                }
            }
    } else if (selection != -1) {
        QMutableVectorIterator<QPair<int, int>> i(validMoves);

        while (i.hasNext()) {
            if (i.next().first != selection)
                i.remove();
        }

        if (!validMoves.empty() && lastMoveIsEat)
            lockSelection = true;
    }
}

QVector<QPair<int, int>> Engine::parseMoves(const Board &a, const Board &b) {
    QVector<QPair<int, int>> moves;

    int location = -1, newLocation = -1;
    int x, y;

    for (int i = 0; i < Board::size(); i++)
        if ((a.at(i) == 'x' || a.at(i) == 'X') && b.at(i) == '_') {
            location = i;
            x = Board::getX(location);
            y = Board::getY(location);
            break;
        }

    char type = a.at(location);

    QSet<QPair<int, int>> eaten;

    while (location != -1) {
        if (a.at(x + 1, y + 1) == '_' && (b.at(x + 1, y + 1) == 'x' || b.at(x + 1, y + 1) == 'X'))
            moves << qMakePair(location, newLocation = Board::indexAt(x + 1, y + 1));
        else if (a.at(x - 1, y + 1) == '_' && (b.at(x - 1, y + 1) == 'x' || b.at(x - 1, y + 1) == 'X'))
            moves << qMakePair(location, newLocation = Board::indexAt(x - 1, y + 1));
        else if ((a.at(x + 1, y + 1) == 'o' || a.at(x + 1, y + 1) == 'O') && b.at(x + 1, y + 1) == '_' && !eaten.contains(qMakePair(x + 1, y + 1))) {
            moves << qMakePair(location, newLocation = Board::indexAt(x + 2, y + 2));
            eaten.insert(qMakePair(x + 1, y + 1));
        } else if ((a.at(x - 1, y + 1) == 'o' || a.at(x - 1, y + 1) == 'O') && b.at(x - 1, y + 1) == '_' && !eaten.contains(qMakePair(x - 1, y + 1))) {
            moves << qMakePair(location, newLocation = Board::indexAt(x - 2, y + 2));
            eaten.insert(qMakePair(x - 1, y + 1));
        } else if (type == 'X') {
            if (a.at(x + 1, y - 1) == '_' && b.at(x + 1, y - 1) == 'X')
                moves << qMakePair(location, newLocation = Board::indexAt(x + 1, y - 1));
            else if (a.at(x - 1, y - 1) == '_' && b.at(x - 1, y - 1) == 'X')
                moves << qMakePair(location, newLocation = Board::indexAt(x - 1, y - 1));
            else if ((a.at(x + 1, y - 1) == 'o' || a.at(x + 1, y - 1) == 'O') && b.at(x + 1, y - 1) == '_' && !eaten.contains(qMakePair(x + 1, y - 1))) {
                moves << qMakePair(location, newLocation = Board::indexAt(x + 2, y - 2));
                eaten.insert(qMakePair(x + 1, y - 1));
            } else if ((a.at(x - 1, y - 1) == 'o' || a.at(x - 1, y - 1) == 'O') && b.at(x - 1, y - 1) == '_' && !eaten.contains(qMakePair(x - 1, y - 1))) {
                moves << qMakePair(location, newLocation = Board::indexAt(x - 2, y - 2));
                eaten.insert(qMakePair(x - 1, y - 1));
            }
        }

        location = newLocation;
        x = Board::getX(location);
        y = Board::getY(location);

        newLocation = -1;
    }

    return moves;
}

bool Engine::isMoveValid(int from, int to) const {
    return validMoves.contains(qMakePair(from, to));
}
