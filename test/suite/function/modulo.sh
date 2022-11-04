suite 'function/modulo'

verify '@write(@freeze(@modulo(1, 2)), 1)' \
    'prints to stdout' '1'
verify '@write(@freeze(@modulo(123, 456)), 1)' \
    'prints to stdout' '123'
verify '@write(@freeze(@modulo(-1, -2)), 1)' \
    'prints to stdout' '-1'
verify '@write(@freeze(@modulo(-123, -456)), 1)' \
    'prints to stdout' '-123'
verify '@write(@freeze(@modulo(123.456, 123.456)), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(@modulo(-123.456, -123.456)), 1)' \
    'prints to stdout' '0'

verify '@modulo(100, 0)' \
    'errors with execute message' '"arithmetic error"'
