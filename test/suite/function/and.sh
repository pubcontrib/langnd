suite 'function/and'

verify '@write(@freeze(@and(false, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@and(false, true)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@and(true, false)), 1)' \
    'prints to stdout' 'false'
verify '@write(@freeze(@and(true, true)), 1)' \
    'prints to stdout' 'true'
