suite 'function/unset'

verify '@write(@freeze(@unset("abc", 1)), 1)' \
    'prints to stdout' '"bc"'
verify '@write(@freeze(@unset("abc", 2)), 1)' \
    'prints to stdout' '"ac"'
verify '@write(@freeze(@unset("abc", 3)), 1)' \
    'prints to stdout' '"ab"'
verify '@write(@freeze(@unset(["a", "b", "c"], 1)), 1)' \
    'prints to stdout' '["b", "c"]'
verify '@write(@freeze(@unset(["a", "b", "c"], 2)), 1)' \
    'prints to stdout' '["a", "c"]'
verify '@write(@freeze(@unset(["a", "b", "c"], 3)), 1)' \
    'prints to stdout' '["a", "b"]'

verify '@write(@freeze(@unset("abc", 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@unset("abc", 4)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@unset(["a", "b", "c"], 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@unset(["a", "b", "c"], 4)), 1)' \
    'errors with execute message' '"absent key"'
