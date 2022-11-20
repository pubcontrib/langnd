suite 'function/add'

verify '@write(@freeze(@add(0, 0)), 1)' \
    'prints to stdout' '0'
verify '@write(@freeze(@add(1, 2)), 1)' \
    'prints to stdout' '3'
verify '@write(@freeze(@add(123, 456)), 1)' \
    'prints to stdout' '579'
verify '@write(@freeze(@add(-1, -2)), 1)' \
    'prints to stdout' '-3'
verify '@write(@freeze(@add(-123, -456)), 1)' \
    'prints to stdout' '-579'
verify '@write(@freeze(@add(0.1, 0.2)), 1)' \
    'prints to stdout' '0.299987'
verify '@write(@freeze(@add(0.123, 0.456)), 1)' \
    'prints to stdout' '0.578979'
verify '@write(@freeze(@add(-0.1, -0.2)), 1)' \
    'prints to stdout' '-0.299987'
verify '@write(@freeze(@add(-0.123, -0.456)), 1)' \
    'prints to stdout' '-0.578979'
verify '@write(@freeze(@add(123.456, 123.456)), 1)' \
    'prints to stdout' '246.911987'
verify '@write(@freeze(@add(-123.456, -123.456)), 1)' \
    'prints to stdout' '-246.911987'

verify '@add(32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify '@add(1, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify '@add(32767, 1)' \
    'errors with execute message' '"arithmetic error"'
