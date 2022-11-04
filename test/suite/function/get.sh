suite 'function/get'

verify '@write(@freeze(@get("abc", 1)), 1)' \
    'prints to stdout' '"a"'
verify '@write(@freeze(@get("abc", 2)), 1)' \
    'prints to stdout' '"b"'
verify '@write(@freeze(@get("abc", 3)), 1)' \
    'prints to stdout' '"c"'

verify '@write(@freeze(@get("abc", 0)), 1)' \
    'errors with execute message' '"absent key"'
verify '@write(@freeze(@get("abc", 4)), 1)' \
    'errors with execute message' '"absent key"'
