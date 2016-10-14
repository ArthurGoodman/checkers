next_player(x, o).
next_player(o, x).

turn_to_sign(x, x).
turn_to_sign(x, y).
turn_to_sign(o, o).
turn_to_sign(o, p).

enemy(x, o).
enemy(x, p).
enemy(y, o).
enemy(y, p).
enemy(o, x).
enemy(o, y).
enemy(p, x).
enemy(p, y).

init(Board) :- Board = b(n,x,n,x,n,x,n,x,
                         x,n,x,n,x,n,x,n,
                         n,x,n,x,n,x,n,x,
                         e,n,e,n,e,n,e,n,
                         n,e,n,e,n,e,n,e,
                         o,n,o,n,o,n,o,n,
                         n,o,n,o,n,o,n,o,
                         o,n,o,n,o,n,o,n).

getPos(Term, Y, X, Sign) :-
    Index is (Y - 1) * 8 + X,
    arg(Index, Term, Sign).

putSign(Board, 8, X, x, NewBoard) :- putSign(Board, 8, X, y, NewBoard), !.
putSign(Board, 1, X, o, NewBoard) :- putSign(Board, 1, X, p, NewBoard), !.

putSign(Board, Y, X, Sign, NewBoard) :-
    Index is (Y - 1) * 8 + X,
    Board =.. [b | List],
    replace(List, Index, Sign, NewList),
    NewBoard =.. [b | NewList].

getPawn(Board, Y, X, P) :-
    getPos(Board, Y, X, P),
    (P = x ; P = y ; P = o ; P = p).

countSigns(Board, Sign, Res) :-
    Board =.. [b | List],
    count(List, Sign, Res).

goal(Board, Winner) :-
    next_player(Winner, Looser),
    findall(NewBoard, validMove(Board, Looser, NewBoard), []).

findPawn(Board, S, Y, X) :-
    arg(Index, Board, S),
    Temp is Index / 8,
    ceiling(Temp, Y),
    X is Index - (Y - 1) * 8.
