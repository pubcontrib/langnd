suite 'core/sort'

verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([], "+")), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([null, null], "+")), 1)' \
    'prints to stdout' '[null, null]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([false, true], "+")), 1)' \
    'prints to stdout' '[false, true]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([true, false], "+")), 1)' \
    'prints to stdout' '[false, true]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([1, 2, 3], "+")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([3, 2, 1], "+")), 1)' \
    'prints to stdout' '[1, 2, 3]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort(["A", "B", "C"], "+")), 1)' \
    'prints to stdout' '["A", "B", "C"]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort(["C", "B", "A"], "+")), 1)' \
    'prints to stdout' '["A", "B", "C"]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([[1], [2], [3]], "+")), 1)' \
    'prints to stdout' '[[1], [2], [3]]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([[3], [2], [1]], "+")), 1)' \
    'prints to stdout' '[[1], [2], [3]]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([{"A": null}, {"B": null}, {"C": null}], "+")), 1)' \
    'prints to stdout' '[{"A": null}, {"B": null}, {"C": null}]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([{"C": null}, {"B": null}, {"A": null}], "+")), 1)' \
    'prints to stdout' '[{"A": null}, {"B": null}, {"C": null}]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<1>, <2>, <3>], "+")), 1)' \
    'prints to stdout' '[<1>, <2>, <3>]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<3>, <2>, <1>], "+")), 1)' \
    'prints to stdout' '[<1>, <2>, <3>]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([null, false, 0, "", [], {}, <>], "+")), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}, <>]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<>, {}, [], "", 0, false, null], "+")), 1)' \
    'prints to stdout' '[null, false, 0, "", [], {}, <>]'

verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([], "-")), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([null, null], "-")), 1)' \
    'prints to stdout' '[null, null]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([false, true], "-")), 1)' \
    'prints to stdout' '[true, false]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([true, false], "-")), 1)' \
    'prints to stdout' '[true, false]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([1, 2, 3], "-")), 1)' \
    'prints to stdout' '[3, 2, 1]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([3, 2, 1], "-")), 1)' \
    'prints to stdout' '[3, 2, 1]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort(["A", "B", "C"], "-")), 1)' \
    'prints to stdout' '["C", "B", "A"]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort(["C", "B", "A"], "-")), 1)' \
    'prints to stdout' '["C", "B", "A"]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([[1], [2], [3]], "-")), 1)' \
    'prints to stdout' '[[3], [2], [1]]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([[3], [2], [1]], "-")), 1)' \
    'prints to stdout' '[[3], [2], [1]]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([{"A": null}, {"B": null}, {"C": null}], "-")), 1)' \
    'prints to stdout' '[{"C": null}, {"B": null}, {"A": null}]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([{"C": null}, {"B": null}, {"A": null}], "-")), 1)' \
    'prints to stdout' '[{"C": null}, {"B": null}, {"A": null}]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<1>, <2>, <3>], "-")), 1)' \
    'prints to stdout' '[<3>, <2>, <1>]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<3>, <2>, <1>], "-")), 1)' \
    'prints to stdout' '[<3>, <2>, <1>]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([null, false, 0, "", [], {}, <>], "-")), 1)' \
    'prints to stdout' '[<>, {}, [], "", 0, false, null]'
verify 'import ["write", "freeze", "sort"] from core $write($freeze($sort([<>, {}, [], "", 0, false, null], "-")), 1)' \
    'prints to stdout' '[<>, {}, [], "", 0, false, null]'

verify 'import "sort" from core $sort([3, 2, 1], "?")' \
    'errors with execute message' '"damaged argument"'
