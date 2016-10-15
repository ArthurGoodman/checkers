heuristic(_/Board, Res) :-
    countSigns(Board, 'x', Res1),
    countSigns(Board, 'o', Res2),
    countSigns(Board, 'X', Res1k),
    countSigns(Board, 'O', Res2k),
    kingBonus(Board, Bonus),
    Res is (Res1 + (Res1k * 1.4)) - (Res2 + (Res2k * 1.4)) + Bonus.

kingBonus(Board, Bonus) :-
    findall(X/Y, findSign(Board, 'X', X, Y), List), !,
    kingBonusL(List, Bonus, 0).

kingBonusL([], Bonus, Bonus).
kingBonusL([X/Y | Tail], Bonus, Agg) :-
    ((Y > 2, Y < 7, B1 is 0.4, !) ; B1 is 0),
    ((X > 2, X < 7, B2 is 0.2, !) ; B2 is 0),
    Agg1 is Agg + B1 + B2,
    kingBonusL(Tail, Bonus, Agg1).
