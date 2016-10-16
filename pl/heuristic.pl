heuristic(_/Board, Value) :-
    countSigns(Board, 'x', AIPawns),
    countSigns(Board, 'X', AIKings),
    countSigns(Board, 'o', PlayerPawns),
    countSigns(Board, 'O', PlayerKings),
    kingBonus(Board, Bonus),
    Value is AIPawns + (AIKings * 1.4) - (PlayerPawns + (PlayerKings * 1.4)) + Bonus.

kingBonus(Board, Bonus) :-
    findall(X/Y, findSign(Board, 'X', X, Y), List), !,
    kingBonusL(List, Bonus, 0).

kingBonusL([], Bonus, Bonus).
kingBonusL([X/Y | Tail], Bonus, Sum) :-
    ((X > 2, X < 7, B1 is 0.2, !) ; B1 is 0),
    ((Y > 2, Y < 7, B2 is 0.4, !) ; B2 is 0),
    NewSum is Sum + B1 + B2,
    kingBonusL(Tail, Bonus, NewSum).
