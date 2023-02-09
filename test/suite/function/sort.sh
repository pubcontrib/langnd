suite 'function/sort'

verify '@write(@freeze(@sort([], "+")), 1)' \
    'prints to stdout' '[]'
verify '@write(@freeze(@sort([null, null], "+")), 1)' \
    'prints to stdout' '[null, null]'
verify '@write(@freeze(@sort([false, true], "+")), 1)' \
    'prints to stdout' '[false, true]'
verify '@write(@freeze(@sort([true, false], "+")), 1)' \
    'prints to stdout' '[false, true]'
verify '@write(@freeze(@sort([1, 2, 3], "+")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify '@write(@freeze(@sort([3, 2, 1], "+")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify '@write(@freeze(@sort(["A", "B", "C"], "+")), 1)' \
    'prints to stdout' '["A", "B", "C"]'
verify '@write(@freeze(@sort(["C", "B", "A"], "+")), 1)' \
    'prints to stdout' '["A", "B", "C"]'
verify '@write(@freeze(@sort([[1], [2], [3]], "+")), 1)' \
    'prints to stdout' '[[1], [2], [3]]'
verify '@write(@freeze(@sort([[3], [2], [1]], "+")), 1)' \
    'prints to stdout' '[[1], [2], [3]]'
verify '@write(@freeze(@sort([{"A": null}, {"B": null}, {"C": null}], "+")), 1)' \
    'prints to stdout' '[{"A": null}, {"B": null}, {"C": null}]'
verify '@write(@freeze(@sort([{"C": null}, {"B": null}, {"A": null}], "+")), 1)' \
    'prints to stdout' '[{"A": null}, {"B": null}, {"C": null}]'
verify '@write(@freeze(@sort([<1>, <2>, <3>], "+")), 1)' \
    'prints to stdout' '[<1>, <2>, <3>]'
verify '@write(@freeze(@sort([<3>, <2>, <1>], "+")), 1)' \
    'prints to stdout' '[<1>, <2>, <3>]'
verify '@write(@freeze(@sort([null, false, 0, "", [], {}, <>], "+")), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}, <>]'
verify '@write(@freeze(@sort([<>, {}, [], "", 0, false, null], "+")), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}, <>]'

verify '@write(@freeze(@sort([], "-")), 1)' \
    'prints to stdout' '[]'
verify '@write(@freeze(@sort([null, null], "-")), 1)' \
    'prints to stdout' '[null, null]'
verify '@write(@freeze(@sort([false, true], "-")), 1)' \
    'prints to stdout' '[true, false]'
verify '@write(@freeze(@sort([true, false], "-")), 1)' \
    'prints to stdout' '[true, false]'
verify '@write(@freeze(@sort([1, 2, 3], "-")), 1)' \
    'prints to stdout' '[3, 2, 1]'
verify '@write(@freeze(@sort([3, 2, 1], "-")), 1)' \
    'prints to stdout' '[3, 2, 1]'
verify '@write(@freeze(@sort(["A", "B", "C"], "-")), 1)' \
    'prints to stdout' '["C", "B", "A"]'
verify '@write(@freeze(@sort(["C", "B", "A"], "-")), 1)' \
    'prints to stdout' '["C", "B", "A"]'
verify '@write(@freeze(@sort([[1], [2], [3]], "-")), 1)' \
    'prints to stdout' '[[3], [2], [1]]'
verify '@write(@freeze(@sort([[3], [2], [1]], "-")), 1)' \
    'prints to stdout' '[[3], [2], [1]]'
verify '@write(@freeze(@sort([{"A": null}, {"B": null}, {"C": null}], "-")), 1)' \
    'prints to stdout' '[{"C": null}, {"B": null}, {"A": null}]'
verify '@write(@freeze(@sort([{"C": null}, {"B": null}, {"A": null}], "-")), 1)' \
    'prints to stdout' '[{"C": null}, {"B": null}, {"A": null}]'
verify '@write(@freeze(@sort([<1>, <2>, <3>], "-")), 1)' \
    'prints to stdout' '[<3>, <2>, <1>]'
verify '@write(@freeze(@sort([<3>, <2>, <1>], "-")), 1)' \
    'prints to stdout' '[<3>, <2>, <1>]'
verify '@write(@freeze(@sort([null, false, 0, "", [], {}, <>], "-")), 1)' \
    'prints to stdout' '[<>, {}, [], "", 0, false, null]'
verify '@write(@freeze(@sort([<>, {}, [], "", 0, false, null], "-")), 1)' \
    'prints to stdout' '[<>, {}, [], "", 0, false, null]'

verify '@sort([3, 2, 1], "?")' \
    'errors with execute message' '"invalid direction"'
