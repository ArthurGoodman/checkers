% move a pawn from one location to another location
move(Board, FromL, FromC, ToL, ToC, NewBoard) :-
    getPawn(Board, FromL, FromC, P),
    turn_to_sign(T, P),!,
    validMove(Board, T, NewBoard), % Check if there is an eat constraint on the board
    (movePawnEatRec(Board, P, FromL, FromC, ToL, ToC, NewBoard) ;
    movePawn(Board, P, FromL, FromC, ToL, ToC, NewBoard)).

% Perform a standard move of a pawn
movePawn(Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
    validateMove(Board, Pawn, FromL, FromC, ToL, ToC),
    putSign(Board, FromL, FromC, e, TB),
    putSign(TB, ToL, ToC, Pawn, NewBoard).

% Perform an eating move of a pawn recursively
movePawnEatRec(Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
    movePawnEat(Board, Pawn, FromL, FromC, ToL, ToC, NewBoard).

movePawnEatRec(Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
    ((Pawn = x ; Pawn = y ; Pawn = p),
    FromL1 is FromL + 2 ;
    (Pawn = o ; Pawn = y ; Pawn = p),
    FromL1 is FromL - 2),
    FromC1 is FromC + 2,
    FromC2 is FromC - 2,
    (movePawnEat(Board, Pawn, FromL, FromC, FromL1, FromC1, TempBoard),
    movePawnEatRec(TempBoard, Pawn, FromL1, FromC1, ToL, ToC, NewBoard) ;
    movePawnEat(Board, Pawn, FromL, FromC, FromL1, FromC2, TempBoard),
    movePawnEatRec(TempBoard, Pawn, FromL1, FromC2, ToL, ToC, NewBoard)).

% Perform a standard eating move of a pawn
movePawnEat(Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
    validateEat(Board, Pawn, FromL, FromC, ToL, ToC),
    getPos(Board, ToL, ToC, e),
    EC1 is (FromC + ToC) / 2,
    EL1 is (FromL + ToL) / 2,
    abs(EC1, EC), abs(EL1, EL),
    putSign(Board, FromL, FromC, e, TB1),
    putSign(TB1, EL, EC, e, TB2),
    putSign(TB2, ToL, ToC, Pawn, NewBoard).

% Check if a specific move is a valid eat
validateEat(Board, King, FromL, FromC, ToL, ToC) :-
    (King = y ; King = p),
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    (ToL is FromL - 2 ; ToL is FromL + 2),
    (ToC is FromC + 2 ; ToC is FromC - 2),
    EL is (ToL + FromL) / 2,
    EC is (ToC + FromC) / 2,
    enemy(King, Enemy),
    getPawn(Board, EL, EC, Enemy).
             
validateEat(Board, x, FromL, FromC, ToL, ToC) :-
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    ToL is FromL + 2,
    (ToC is FromC + 2 ; ToC is FromC - 2),
    EL is (ToL + FromL) / 2,
    EC is (ToC + FromC) / 2,
    enemy(x, Enemy),
    getPawn(Board, EL, EC, Enemy).

validateEat(Board, o, FromL, FromC, ToL, ToC) :-
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    ToL is FromL - 2,
    (ToC is FromC + 2 ; ToC is FromC - 2),
    EL is (ToL + FromL) / 2,
    EC is (ToC + FromC) / 2,
    enemy(o, Enemy),
    getPawn(Board, EL, EC, Enemy).

% Check if a specific move is valid
validateMove(Board, King, FromL, FromC, ToL, ToC) :-
    (King = y ; King = p),
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    (ToL is FromL + 1 ; ToL is FromL - 1),
    (ToC is FromC + 1 ; ToC is FromC - 1),
    getPos(Board, ToL, ToC, e).

validateMove(Board, x, FromL, FromC, ToL, ToC) :-
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    ToL is FromL + 1,
    (ToC is FromC + 1 ; ToC is FromC - 1),
    getPos(Board, ToL, ToC, e).
               
validateMove(Board, o, FromL, FromC, ToL, ToC) :-
    ToL >= 1, ToC >= 1,
    FromL =< 8, FromL =< 8,
    ToL is FromL - 1,
    (ToC is FromC + 1 ; ToC is FromC - 1),
    getPos(Board, ToL, ToC, e).

% Get all the valid eat moves that availible on the board
validEatMove(Board, Sign, NewBoard) :-
    findPawn(Board, Sign, L, C), findPawn(Board, e, Tl, Tc),
    movePawnEatRec(Board, Sign, L, C, Tl, Tc, NewBoard).

% Get all the valid standard moves that availible on the board
validStdMove(Board, Sign, NewBoard) :-
    findPawn(Board, Sign, L, C), findPawn(Board, e, Tl, Tc),
    movePawn(Board, Sign, L, C, Tl, Tc, NewBoard).

% A move on the board is valid if it's an eat move
validMove(Board, Turn, NewBoard) :-
    turn_to_sign(Turn, Sign),
    validEatMove(Board, Sign, NewBoard).

% Or a standard move when no eat moves are availible
validMove(Board, Turn, NewBoard) :-
    not((turn_to_sign(Turn, Sign),
    validEatMove(Board, Sign, NewBoard))),
    turn_to_sign(Turn, Sign1),
    validStdMove(Board, Sign1, NewBoard).

% Get a list of the valid moves that can be on the board
moves(Turn/Board, [Head | Tail]) :-
    next_player(Turn, NextTurn),
    findall(NextTurn/NewBoard, validMove(Board, Turn, NewBoard), [Head | Tail]).
