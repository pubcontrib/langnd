suite 'function/unset'

verify '@write(@freeze(@unset("abc", 1)), 1)' \
    'prints to stdout' '"bc"'
verify '@write(@freeze(@unset("abc", 2)), 1)' \
    'prints to stdout' '"ac"'
verify '@write(@freeze(@unset("abc", 3)), 1)' \
    'prints to stdout' '"ab"'

verify '@write(@freeze(@unset("abc", 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@unset("abc", 4)), 1)' \
    'errors with execute message' '"absent key"'
