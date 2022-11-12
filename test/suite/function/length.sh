suite 'function/length'

verify '@write(@freeze(@length("")), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(@length("0")), 1)' \
    'prints to stdout' '1'
verify '@write(@freeze(@length("0123456789")), 1)' \
    'prints to stdout' '10'
verify '@write(@freeze(@length([])), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(@length([0])), 1)' \
    'prints to stdout' '1'
verify '@write(@freeze(@length([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])), 1)' \
    'prints to stdout' '10'
