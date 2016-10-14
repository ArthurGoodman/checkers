heuristic(_/Board, Res) :-
    countSigns(Board, x, Res1),
    countSigns(Board, o, Res2),
    countSigns(Board, y, Res1k),
    countSigns(Board, p, Res2k),
    king_bonus(Board, y, Bonus),
    Res is (Res1 + (Res1k * 1.4)) - (Res2 + (Res2k * 1.4)) + Bonus.

king_bonus(Board, Sign, Bonus) :-
    findall(X/Y, findPawn(Board, Sign, Y, X), List), !,
    king_bonusL(List, Bonus, 0).

king_bonusL([], Bonus, Bonus).
king_bonusL([X/Y | Tail], Bonus, Agg) :-
    ((Y > 2, Y < 7, B1 is 0.4, !) ; B1 is 0),
    ((X > 2, X < 7, B2 is 0.2, !) ; B2 is 0),
    Agg1 is Agg + B1 + B2,
    king_bonusL(Tail, Bonus, Agg1).
