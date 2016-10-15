move(Board, XFrom, YFrom, XTo, YTo, NewBoard) :-
    getSign(Board, XFrom, YFrom, Pawn),
    turnToSign(Turn, Pawn), !,
    validMove(Board, Turn, NewBoard),
    (movePawnEatRec(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard) ;
    movePawn(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard)).

movePawn(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard) :-
    validateMove(Board, Pawn, XFrom, YFrom, XTo, YTo),
    putSign(Board, XFrom, YFrom, '_', TempBoard),
    putSign(TempBoard, XTo, YTo, Pawn, NewBoard).

movePawnEatRec(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard) :-
    movePawnEat(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard), !.

movePawnEatRec(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard) :-
    ((Pawn = 'x' ; Pawn = 'X' ; Pawn = 'O'), YTemp is YFrom + 2 ;
    (Pawn = 'o' ; Pawn = 'X' ; Pawn = 'O'), YTemp is YFrom - 2),
    XTemp1 is XFrom + 2,
    XTemp2 is XFrom - 2,
    (movePawnEat(Board, Pawn, XFrom, YFrom, XTemp1, YTemp, TempBoard),
    movePawnEatRec(TempBoard, Pawn, XTemp1, YTemp, XTo, YTo, NewBoard) ;
    movePawnEat(Board, Pawn, XFrom, YFrom, XTemp2, YTemp, TempBoard),
    movePawnEatRec(TempBoard, Pawn, XTemp2, YTemp, XTo, YTo, NewBoard)).

movePawnEat(Board, Pawn, XFrom, YFrom, XTo, YTo, NewBoard) :-
    validateEat(Board, Pawn, XFrom, YFrom, XTo, YTo),
    getPos(Board, XTo, YTo, '_'),
    EC1 is (XFrom + XTo) / 2,
    EL1 is (YFrom + YTo) / 2,
    abs(EC1, EC), abs(EL1, EL),
    putSign(Board, XFrom, YFrom, '_', TB1),
    putSign(TB1, EC, EL, '_', TB2),
    putSign(TB2, XTo, YTo, Pawn, NewBoard).

validateEat(Board, King, XFrom, YFrom, XTo, YTo) :-
    (King = 'X' ; King = 'O'),
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    (YTo is YFrom - 2 ; YTo is YFrom + 2),
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    EL is (YTo + YFrom) / 2,
    EC is (XTo + XFrom) / 2,
    enemy(King, Enemy),
    getSign(Board, EC, EL, Enemy).

validateEat(Board, 'x', XFrom, YFrom, XTo, YTo) :-
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    YTo is YFrom + 2,
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    EL is (YTo + YFrom) / 2,
    EC is (XTo + XFrom) / 2,
    enemy('x', Enemy),
    getSign(Board, EC, EL, Enemy).

validateEat(Board, 'o', XFrom, YFrom, XTo, YTo) :-
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    YTo is YFrom - 2,
    (XTo is XFrom + 2 ; XTo is XFrom - 2),
    EL is (YTo + YFrom) / 2,
    EC is (XTo + XFrom) / 2,
    enemy('o', Enemy),
    getSign(Board, EC, EL, Enemy).

validateMove(Board, King, XFrom, YFrom, XTo, YTo) :-
    (King = 'X' ; King = 'O'),
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    (YTo is YFrom + 1 ; YTo is YFrom - 1),
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    getPos(Board, XTo, YTo, '_').

validateMove(Board, 'x', XFrom, YFrom, XTo, YTo) :-
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    YTo is YFrom + 1,
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    getPos(Board, XTo, YTo, '_').

validateMove(Board, 'o', XFrom, YFrom, XTo, YTo) :-
    YTo >= 1, XTo >= 1,
    YFrom =< 8, YFrom =< 8,
    YTo is YFrom - 1,
    (XTo is XFrom + 1 ; XTo is XFrom - 1),
    getPos(Board, XTo, YTo, '_').

validEatMove(Board, Sign, NewBoard) :-
    findSign(Board, Sign, XFrom, YFrom), findSign(Board, '_', XTo, YTo),
    movePawnEatRec(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard).

validStdMove(Board, Sign, NewBoard) :-
    findSign(Board, Sign, XFrom, YFrom), findSign(Board, '_', XTo, YTo),
    movePawn(Board, Sign, XFrom, YFrom, XTo, YTo, NewBoard).

validMove(Board, Turn, NewBoard) :-
    turnToSign(Turn, Sign),
    validEatMove(Board, Sign, NewBoard).

validMove(Board, Turn, NewBoard) :-
    turnToSign(Turn, Sign),
    \+ validEatMove(Board, Sign, NewBoard),
    validStdMove(Board, Sign, NewBoard).

moves(Turn/Board, [Head | Tail]) :-
    nextPlayer(Turn, NextTurn),
    findall(NextTurn/NewBoard, validMove(Board, Turn, NewBoard), [Head | Tail]).
