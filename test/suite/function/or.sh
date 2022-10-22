suite 'function/or'

verify '@write(@freeze(@or(false, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@or(false, true)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@or(true, false)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@or(true, true)), 1)' \
    'prints to stdout' 'true'
