suite 'function/not'

verify '@write(@freeze(@not(false)), 1)' \
    'prints to stdout' 'true'
verify '@write(@freeze(@not(true)), 1)' \
    'prints to stdout' 'false'
