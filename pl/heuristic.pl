% The hueristic function
% The amount of the computers pawns minus the amount of the human pawns
% a king is worth two standard pawns
heuristic(_/Board, Res) :-
    count(Board, x, Res1),
    count(Board, o, Res2),
    count(Board, y, Res1k),
    count(Board, p, Res2k),
    king_bonus(Board, y, Bonus),
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
