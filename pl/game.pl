nextPlayer(x, o).
nextPlayer(o, x).

turnToSign(x, 'x').
turnToSign(x, 'X').
turnToSign(o, 'o').
turnToSign(o, 'O').

enemy('x', 'o').
enemy('x', 'O').
enemy('X', 'o').
enemy('X', 'O').
enemy('o', 'x').
enemy('o', 'X').
enemy('O', 'x').
enemy('O', 'X').

init(Board) :- Board = board(' ','x',' ','x',' ','x',' ','x',

                             'x',' ','x',' ','x',' ','x',' ',

                             ' ','x',' ','x',' ','x',' ','x',

                             '_',' ','_',' ','_',' ','_',' ',

                             ' ','_',' ','_',' ','_',' ','_',

                             'o',' ','o',' ','o',' ','o',' ',

                             ' ','o',' ','o',' ','o',' ','o',

                             'o',' ','o',' ','o',' ','o',' ').

indexAt(X, Y, Index) :- Index is (Y - 1) * 8 + X.

getX(Index, X) :- X is mod(Index - 1, 8) + 1.
getY(Index, Y) :- Y is div(Index - 1, 8) + 1.

getPos(Term, X, Y, Sign) :-
    indexAt(X, Y, Index),
    arg(Index, Term, Sign).

putSign(Board, X, 8, 'x', NewBoard) :- putSign(Board, X, 8, 'X', NewBoard), !.
putSign(Board, X, 1, 'o', NewBoard) :- putSign(Board, X, 1, 'O', NewBoard), !.

putSign(Board, X, Y, Sign, NewBoard) :-
    indexAt(X, Y, Index),
    Board =.. [board | List],
    replace(List, Index, Sign, NewList),
    NewBoard =.. [board | NewList].

getSign(Board, X, Y, Sign) :-
    getPos(Board, X, Y, Sign),
    (Sign = 'x' ; Sign = 'X' ; Sign = 'o' ; Sign = 'O').

findSign(Board, Sign, X, Y) :-
    arg(Index, Board, Sign),
    getX(Index, X),
    getY(Index, Y).

countSigns(Board, Sign, Res) :-
    Board =.. [board | List],
    count(List, Sign, Res).

goal(Board, Winner) :-
    nextPlayer(Winner, Looser),
    findall(NewBoard, validMove(Board, Looser, NewBoard), []).
