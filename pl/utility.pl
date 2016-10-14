getPos(Term, Line, Col, Sign) :- Num is ((Line - 1) * 8) + Col, arg(Num, Term, Sign).

replace([_ | Tail], 1, Value, [Value | Tail]).
replace([Head | Tail], Index, Value, [Head | NewTail]) :- Index > -1, NewIndex is Index - 1, replace(Tail, NewIndex, Value, NewTail), !.
replace(List, _, _, List).
