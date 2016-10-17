move(Board, XFrom, YFrom, XTo, YTo, NewBoard) :-
    getSign(Board, XFrom, YFrom, Sign),
    turnToSign(Turn, Sign), !,
    validMove(Board, Turn, NewBoard),
    (movePawnEatRec(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard) ;
    movePawn(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard)).

movePawn(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard) :-
    validateMove(Board, Sign, XFrom, YFrom, XTo, YTo),
    putSign(Board, XFrom, YFrom, '_', TempBoard),
    putSign(TempBoard, XTo, YTo, Sign, NewBoard).

movePawnEatRec(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard) :-
    movePawnEat(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard).

movePawnEatRec(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard) :-
    ((Sign = 'x' ; Sign = 'X' ; Sign = 'O'), YTemp is YFrom + 2 ;
    (Sign = 'o' ; Sign = 'X' ; Sign = 'O'), YTemp is YFrom - 2),
    XTemp1 is XFrom + 2,
    XTemp2 is XFrom - 2,
    (movePawnEat(Board, Sign, XFrom, YFrom, XTemp1, YTemp, TempBoard),
    movePawnEatRec(TempBoard, Sign, XTemp1, YTemp, XTo, YTo, NewBoard) ;
    movePawnEat(Board, Sign, XFrom, YFrom, XTemp2, YTemp, TempBoard),
    movePawnEatRec(TempBoard, Sign, XTemp2, YTemp, XTo, YTo, NewBoard)).

movePawnEat(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard) :-
    validateEat(Board, Sign, XFrom, YFrom, XTo, YTo),
    XEnemy is (XFrom + XTo) / 2,
    YEnemy is (YFrom + YTo) / 2,
    putSign(Board, XFrom, YFrom, '_', TempBoard1),
    putSign(TempBoard1, XEnemy, YEnemy, '_', TempBoard2),
    putSign(TempBoard2, XTo, YTo, Sign, NewBoard).

inBounds(XFrom, YFrom, XTo, YTo) :-
    XFrom >= 1, YFrom >= 1, XTo >= 1, YTo >= 1,
    XFrom =< 8, YFrom =< 8, XTo =< 8, YTo =< 8.

validateEat(Board, King, XFrom, YFrom, XTo, YTo) :-
    (King = 'X' ; King = 'O'),
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    (YTo is YFrom - 2 ; YTo is YFrom + 2),
    XEnemy is (XTo + XFrom) / 2,
    YEnemy is (YTo + YFrom) / 2,
    enemy(King, Enemy),
    getSign(Board, XEnemy, YEnemy, Enemy),
    getPos(Board, XTo, YTo, '_').

validateEat(Board, 'x', XFrom, YFrom, XTo, YTo) :-
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    YTo is YFrom + 2,
    XEnemy is (XTo + XFrom) / 2,
    YEnemy is (YTo + YFrom) / 2,
    enemy('x', Enemy),
    getSign(Board, XEnemy, YEnemy, Enemy),
    getPos(Board, XTo, YTo, '_').

validateEat(Board, 'o', XFrom, YFrom, XTo, YTo) :-
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    YTo is YFrom - 2,
    XEnemy is (XTo + XFrom) / 2,
    YEnemy is (YTo + YFrom) / 2,
    enemy('o', Enemy),
    getSign(Board, XEnemy, YEnemy, Enemy),
    getPos(Board, XTo, YTo, '_').

validateMove(Board, King, XFrom, YFrom, XTo, YTo) :-
    (King = 'X' ; King = 'O'),
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    (YTo is YFrom + 1 ; YTo is YFrom - 1),
    getPos(Board, XTo, YTo, '_').

validateMove(Board, 'x', XFrom, YFrom, XTo, YTo) :-
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    YTo is YFrom + 1,
    getPos(Board, XTo, YTo, '_').

validateMove(Board, 'o', XFrom, YFrom, XTo, YTo) :-
    inBounds(XFrom, YFrom, XTo, YTo),
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    YTo is YFrom - 1,
    getPos(Board, XTo, YTo, '_').

validEatMove(Board, Sign, NewBoard) :-
    findSign(Board, Sign, XFrom, YFrom), findSign(Board, '_', XTo, YTo),
    movePawnEatRec(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard).

validStdMove(Board, Sign, NewBoard) :-
    findSign(Board, Sign, XFrom, YFrom), findSign(Board, '_', XTo, YTo),
    movePawn(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard).

validMove(Board, Turn, NewBoard) :-
    turnToSign(Turn, Sign), validEatMove(Board, Sign, NewBoard).

validMove(Board, Turn, NewBoard) :-
    \+ (turnToSign(Turn, S), validEatMove(Board, S, NewBoard)),
    turnToSign(Turn, Sign),
    validStdMove(Board, Sign, NewBoard).

moves(Turn/Board, Positions) :-
    Positions = [Head | Tail],
    nextPlayer(Turn, NextTurn),
    findall(NextTurn/NewBoard, validMove(Board, Turn, NewBoard), Positions).
