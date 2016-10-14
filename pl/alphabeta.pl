alphabeta(Pos, Alpha, Beta, GoodPos, Val, Depth) :-
    Depth > 0, moves(Pos, Positions), !,
    boundedBest(Positions, Alpha, Beta, GoodPos, Val, Depth);
    heuristic(Pos, Val).

boundedBest([Pos | Positions], Alpha, Beta, GoodPos, GoodVal, Depth) :-
    NewDepth is Depth - 1,
    alphabeta(Pos, Alpha, Beta, _, Val, NewDepth),
    goodEnough(Positions, Alpha, Beta, Pos, Val, GoodPos, GoodVal, Depth).

goodEnough([], _, _, Pos, Val, Pos, Val, _).

goodEnough(_, Alpha, Beta, o/_, Val, o/_, Val, _) :- Val > Beta.
goodEnough(_, Alpha, Beta, x/_, Val, x/_, Val, _) :- Val < Alpha.

goodEnough(Positions, Alpha, Beta, Pos, Val, GoodPos, GoodVal, Depth) :-
    newBounds(Alpha, Beta, Pos, Val, NewAlpha, NewBeta),
    boundedBest(Positions, NewAlpha, NewBeta, Pos1, Val1, Depth),
    bestPosition(Pos, Val, Pos1, Val1, GoodPos, GoodVal).

newBounds(Alpha, Beta, o/_, Val, Val, Beta) :- Val > Alpha.
newBounds(Alpha, Beta, x/_, Val, Alpha, Val) :- Val < Beta.
newBounds(Alpha, Beta, _, _, Alpha, Beta).

bestPosition(o/Board, Val1, _, Val2, o/Board, Val1) :- Val1 > Val2.
bestPosition(x/Board, Val1, _, Val2, x/Board, Val1) :- Val1 < Val2.
bestPosition(_, _, Pos2, Val2, Pos2, Val2).
