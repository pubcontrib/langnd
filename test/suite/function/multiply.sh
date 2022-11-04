suite 'function/multiply'

verify '@write(@freeze(@multiply(0, 0)), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(@multiply(1, 2)), 1)' \
    'prints to stdout' '2'
verify '@write(@freeze(@multiply(-1, -2)), 1)' \
    'prints to stdout' '2'
verify '@write(@freeze(@multiply(0.1, 0.2)), 1)' \
    'prints to stdout' '0.019989'
verify '@write(@freeze(@multiply(0.123, 0.456)), 1)' \
    'prints to stdout' '0.056076'
verify '@write(@freeze(@multiply(-0.1, -0.2)), 1)' \
    'prints to stdout' '0.019989'
verify '@write(@freeze(@multiply(-0.123, -0.456)), 1)' \
    'prints to stdout' '0.056076'
verify '@write(@freeze(@multiply(123.456, 123.456)), 1)' \
    'prints to stdout' '15241.382354'
verify '@write(@freeze(@multiply(-123.456, -123.456)), 1)' \
    'prints to stdout' '15241.382354'

verify '@multiply(32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify '@multiply(32767, -32767)' \
    'errors with execute message' '"arithmetic error"'
verify '@multiply(-32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify '@multiply(-32767, -32767)' \
    'errors with execute message' '"arithmetic error"'
