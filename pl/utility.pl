replace([_ | Tail], 1, Value, [Value | Tail]).
replace([Head | Tail], Index, Value, [Head | NewTail]) :- Index > -1, NewIndex is Index - 1, replace(Tail, NewIndex, Value, NewTail), !.
replace(List, _, _, List).

count([], _, 0).
count([Head | Tail], Head, Count) :- count(Tail, Head, C), Count is C + 1.
count([Head | Tail], Element, Count) :- count(Tail, Element, Count), Head \= Element.
