getPos( Board, Line, Col, Sign) :-
        Num is ((Line - 1) * 8) + Col,
        arg(Num, Board, Sign).

%
% Basic relations
%

% Who's the next turn?
next_player(x, o).
next_player(o, x).

% convert a turn sign to it's pawns
turn_to_sign(x, x).
turn_to_sign(x, y).
turn_to_sign(o, o).
turn_to_sign(o, p).

% convert a pawn to a king sign
king_sign(x, y).
king_sign(o, p).

% the enemies pawns of each pawn
enemy(o, x).
enemy(o, y).
enemy(x, o).
enemy(x, p).
enemy(p, x).
enemy(p, y).
enemy(y, o).
enemy(y, p).

% The initial board
init( Board) :-
      Board = b(n,x,n,x,n,x,n,x,x,n,x,n,x,n,x,n,n,x,n,x,n,x,n,x,e,n,e,n,e,n,e,n,n,e,n,e,n,e,n,e,o,n,o,n,o,n,o,n,n,o,n,o,n,o,n,o,o,n,o,n,o,n,o,n).

% clears the asserts when the game ends
clear :-
      retractall(max_to_move(_)),
      retractall(min_to_move(_)),!.

% put a sign in a specific location in the board relation
putSign( Board, 8, Col, x, NewBoard) :-
         putSign(Board, 8, Col, y, NewBoard),!.
         
putSign( Board, 1, Col, o, NewBoard) :-
         putSign(Board, 1, Col, p, NewBoard),!.
         
putSign( Board, Line, Col, Sign, NewBoard) :-
         Place is ((Line - 1) * 8) + Col,
         Board =.. [b|List],
         replace(List, Place, Sign, NewList),
         NewBoard =.. [b|NewList].
         
% replace an atom in a specific location in a list
% with another atom.
replace( List, Place, Val, NewList) :-
         replace(List, Place, Val, NewList, 1).
replace( [], _, _, [], _).
replace( [_|Xs], Place, Val, [Val|Ys], Place) :-
         NewCounter is Place + 1, !,
         replace(Xs, Place, Val, Ys, NewCounter).

replace( [X|Xs], Place, Val, [X|Ys], Counter) :-
         NewCounter is Counter + 1,
         replace(Xs, Place, Val, Ys, NewCounter).
         
getPawn( Board, Line, Col, P) :-
         getPos( Board, Line, Col, P),
         (P = x ; P = y ; P = o ; P = p).
         
% counts how many instances of a sign are on the board
count( Board, Sign, Res) :-
       Board =.. [b|List],
       countL(List, Sign, Res, 0).

countL( [], _, Res, Res) :- !.
countL( [Sign|Xs], Sign, Res, Counter) :-
        !, Counter1 is Counter + 1,
        countL(Xs, Sign, Res, Counter1).
countL( [_|Xs], Sign, Res, Counter) :-
        countL(Xs, Sign, Res, Counter).
        
%
% Goal relation
%

% Check if someone won the game
goal( Board, Winner) :-
      next_player(Winner, Looser),
      findall(NewBoard, (turn_to_sign(Looser,Sign),validMove(Board, Sign, NewBoard)), []),!.

%
% Move relations
%

% move a pawn from one location to another location
move( Board, FromL, FromC, ToL, ToC, NewBoard) :-
      getPawn(Board, FromL, FromC, P),
      turn_to_sign(T, P),!,
      validMove(Board, T, NewBoard), % Check if there is an eat constraint on the board
      (movePawnEatRec(Board, P, FromL, FromC, ToL, ToC, NewBoard) ;
      movePawn(Board, P, FromL, FromC, ToL, ToC, NewBoard)).

% Perform a standard move of a pawn
movePawn( Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
          validateMove(Board, Pawn, FromL, FromC, ToL, ToC),
          putSign(Board, FromL, FromC, e, TB),
          putSign(TB, ToL, ToC, Pawn, NewBoard).

% Perform an eating move of a pawn recursively
movePawnEatRec( Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
          movePawnEat( Board, Pawn, FromL, FromC, ToL, ToC, NewBoard).
          
movePawnEatRec( Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
          ((Pawn = x ; Pawn = y ; Pawn = p),
          FromL1 is FromL + 2 ;
          (Pawn = o ; Pawn = y ; Pawn = p),
          FromL1 is FromL - 2),
          FromC1 is FromC + 2,
          FromC2 is FromC - 2,
          (movePawnEat( Board, Pawn, FromL, FromC, FromL1, FromC1, TempBoard),
          movePawnEatRec( TempBoard, Pawn, FromL1, FromC1, ToL, ToC, NewBoard) ;
          movePawnEat( Board, Pawn, FromL, FromC, FromL1, FromC2, TempBoard),
          movePawnEatRec( TempBoard, Pawn, FromL1, FromC2, ToL, ToC, NewBoard)).

% Perform a standard eating move of a pawn
movePawnEat( Board, Pawn, FromL, FromC, ToL, ToC, NewBoard) :-
          validateEat(Board, Pawn, FromL, FromC, ToL, ToC),
          getPos(Board, ToL, ToC, e),
          EC1 is (FromC + ToC) / 2,
          EL1 is (FromL + ToL) / 2,
          abs(EC1, EC), abs(EL1, EL),
          putSign(Board, FromL, FromC, e, TB1),
          putSign(TB1, EL, EC, e, TB2),
          putSign(TB2, ToL, ToC, Pawn, NewBoard).

% Check if a specific move is a valid eat
validateEat( Board, King, FromL, FromC, ToL, ToC) :-
             (King = y ; King = p),
             ToL >= 1, ToC >= 1,
             FromL =< 8, FromL =< 8,
             (ToL is FromL - 2 ;
              ToL is FromL + 2),
             (ToC is FromC + 2 ;
              ToC is FromC - 2),
             EL is (ToL + FromL) / 2,
             EC is (ToC + FromC) / 2,
             enemy(King, Enemy),
             getPawn(Board, EL, EC, Enemy).
             
validateEat( Board, x, FromL, FromC, ToL, ToC) :-
             ToL >= 1, ToC >= 1,
             FromL =< 8, FromL =< 8,
             ToL is FromL + 2,
             (ToC is FromC + 2 ;
              ToC is FromC - 2),
              EL is (ToL + FromL) / 2,
              EC is (ToC + FromC) / 2,
              enemy(x, Enemy),
              getPawn(Board, EL, EC, Enemy).

validateEat( Board, o, FromL, FromC, ToL, ToC) :-
             ToL >= 1, ToC >= 1,
             FromL =< 8, FromL =< 8,
             ToL is FromL - 2,
             (ToC is FromC + 2 ;
              ToC is FromC - 2),
              EL is (ToL + FromL) / 2,
              EC is (ToC + FromC) / 2,
              enemy(o, Enemy),
              getPawn(Board, EL, EC, Enemy).

% Check if a specific move is valid
validateMove( Board, King, FromL, FromC, ToL, ToC) :-
              (King = y ; King = p),
              ToL >= 1, ToC >= 1,
              FromL =< 8, FromL =< 8,
              (ToL is FromL + 1 ;
               ToL is FromL - 1),
              (ToC is FromC + 1 ;
               ToC is FromC - 1),
               getPos(Board, ToL, ToC, e).

validateMove( Board, x, FromL, FromC, ToL, ToC) :-
              ToL >= 1, ToC >= 1,
              FromL =< 8, FromL =< 8,
              ToL is FromL + 1,
              (ToC is FromC + 1 ;
               ToC is FromC - 1),
               getPos(Board, ToL, ToC, e).
               
validateMove( Board, o, FromL, FromC, ToL, ToC) :-
              ToL >= 1, ToC >= 1,
              FromL =< 8, FromL =< 8,
              ToL is FromL - 1,
              (ToC is FromC + 1 ;
               ToC is FromC - 1),
               getPos(Board, ToL, ToC, e).

% Gets a board and a place in the array
% and returns the line and column of it
findPawn( Board, S, Line, Col) :-
          arg(Num, Board, S),
          Temp is Num / 8,
          ceiling(Temp, Line),
          Col is Num - ((Line - 1) * 8).
          
% Get all the valid eat moves that availible on the board
validEatMove( Board, Sign, NewBoard) :-
           findPawn(Board, Sign, L, C),findPawn(Board, e, Tl, Tc),
           movePawnEatRec(Board, Sign, L, C, Tl, Tc, NewBoard).

% Get all the valid standard moves that availible on the board
validStdMove( Board, Sign, NewBoard) :-
              findPawn(Board, Sign, L, C),findPawn(Board, e, Tl, Tc),
              movePawn(Board, Sign, L, C, Tl, Tc, NewBoard).

% A move on the board is valid if it's an eat move
validMove( Board, Turn, NewBoard) :-
           turn_to_sign(Turn, Sign),
           validEatMove(Board, Sign, NewBoard).

% Or a standard move when no eat moves are availible
validMove( Board, Turn, NewBoard) :-
           not((turn_to_sign(Turn, Sign),
           validEatMove(Board, Sign, NewBoard))),
           turn_to_sign(Turn, Sign1),
           validStdMove(Board, Sign1, NewBoard).

%
% Alpha-Beta implementation
%
          
% alphabeta algorithm
alphabeta( Pos, Alpha, Beta, GoodPos, Val, Depth) :-
           Depth > 0, moves( Pos, PosList), !,
           boundedbest( PosList, Alpha, Beta, GoodPos, Val, Depth);
           staticval( Pos, Val).        % Static value of Pos

boundedbest( [Pos|PosList], Alpha, Beta, GoodPos, GoodVal, Depth) :-
             Depth1 is Depth - 1,
             alphabeta( Pos, Alpha, Beta, _, Val, Depth1),
             goodenough( PosList, Alpha, Beta, Pos, Val, GoodPos, GoodVal, Depth).

goodenough( [], _, _, Pos, Val, Pos, Val, _) :- !.     % No other candidate

goodenough( _, Alpha, Beta, Pos, Val, Pos, Val, _) :-
            min_to_move( Pos), Val > Beta, !;       % Maximizer attained upper bound
            max_to_move( Pos), Val < Alpha, !.      % Minimizer attained lower bound

goodenough( PosList, Alpha, Beta, Pos, Val, GoodPos, GoodVal, Depth) :-
            newbounds( Alpha, Beta, Pos, Val, NewAlpha, NewBeta),        % Refine bounds
            boundedbest( PosList, NewAlpha, NewBeta, Pos1, Val1, Depth),
            betterof( Pos, Val, Pos1, Val1, GoodPos, GoodVal).

newbounds( Alpha, Beta, Pos, Val, Val, Beta) :-
           min_to_move( Pos), Val > Alpha, !.        % Maximizer increased lower bound

newbounds( Alpha, Beta, Pos, Val, Alpha, Val) :-
           max_to_move( Pos), Val < Beta, !.         % Minimizer decreased upper bound

newbounds( Alpha, Beta, _, _, Alpha, Beta).          % Otherwise bounds unchanged

betterof( Pos, Val, _, Val1, Pos, Val) :-         % Pos better then Pos1
          min_to_move( Pos), Val > Val1, !;
          max_to_move( Pos), Val < Val1, !.

betterof( _, _, Pos1, Val1, Pos1, Val1).             % Otherwise Pos1 better

%
% Alpha-Beta satellite relations
%

% Get a list of the valid moves that can be on the board
moves( Turn/Board, [X|Xs]) :-
       next_player(Turn, NextTurn),
       findall(NextTurn/NewBoard, validMove(Board, Turn, NewBoard), [X|Xs]).

% The hueristic function
% The amount of the computers pawns minus the amount of the human pawns
% a king is worth two standard pawns
staticval( _/Board, Res) :-
           max_to_move(Comp/_),
           min_to_move(Human/_),
           %next_player(Comp, Human),
           count( Board, Comp, Res1),
           count( Board, Human, Res2),
           king_sign(Comp, CompK),
           king_sign(Human, HumanK),
           count(Board, CompK, Res1k),
           count(Board, HumanK, Res2k),
           king_bonus(Board, CompK, Bonus),
           Res is (Res1 + (Res1k * 1.4)) - (Res2 + (Res2k * 1.4)) + Bonus.
        
king_bonus( Board, Sign, Bonus) :-
            findall(L/C, findPawn(Board, Sign, L, C), List),!,
            king_bonusL( List, Bonus, 0).

king_bonusL( [], Bonus, Bonus).
king_bonusL( [L/C|Xs], Bonus, Agg) :-
             ((L > 2, L < 7, B1 is 0.4,!) ;
             B1 is 0),
             ((C > 2, C < 7, B2 is 0.2,!) ;
             B2 is 0),
             Agg1 is Agg + B1 + B2,
             king_bonusL(Xs, Bonus, Agg1).

% End of file
   