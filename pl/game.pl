next_player(x, o).
next_player(o, x).

turn_to_sign(x, x).
turn_to_sign(x, y).
turn_to_sign(o, o).
turn_to_sign(o, p).

king_sign(x, y).
king_sign(o, p).

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

putSign(Board, 8, Col, x, NewBoard) :- putSign(Board, 8, Col, y, NewBoard), !.
putSign(Board, 1, Col, o, NewBoard) :- putSign(Board, 1, Col, p, NewBoard), !.

putSign(Board, Line, Col, Sign, NewBoard) :-
    Place is ((Line - 1) * 8) + Col,
    Board =.. [b|List],
    replace(List, Place, Sign, NewList),
    NewBoard =.. [b|NewList].

getPawn(Board, Line, Col, P) :-
    getPos(Board, Line, Col, P),
    (P = x ; P = y ; P = o ; P = p).

count(Board, Sign, Res) :-
    Board =.. [b|List],
    countL(List, Sign, Res, 0).

countL([], _, Res, Res) :- !.
countL([Sign|Xs], Sign, Res, Counter) :- !, Counter1 is Counter + 1, countL(Xs, Sign, Res, Counter1).
countL([_|Xs], Sign, Res, Counter) :- countL(Xs, Sign, Res, Counter).

goal(Board, Winner) :-
    next_player(Winner, Looser),
    findall(NewBoard, (turn_to_sign(Looser,Sign),validMove(Board, Sign, NewBoard)), []),!.

% Gets a board and a place in the array
% and returns the line and column of it
findPawn(Board, S, Line, Col) :-
    arg(Num, Board, S),
    Temp is Num / 8,
    ceiling(Temp, Line),
    Col is Num - ((Line - 1) * 8).