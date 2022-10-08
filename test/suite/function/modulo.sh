suite 'function/modulo'

pass '@write(@freeze(@modulo(1, 2)), 1)' '1'
pass '@write(@freeze(@modulo(123, 456)), 1)' '123'
pass '@write(@freeze(@modulo(-1, -2)), 1)' '-1'
pass '@write(@freeze(@modulo(-123, -456)), 1)' '-123'
pass '@write(@freeze(@modulo(123.456, 123.456)), 1)' '0'
pass '@write(@freeze(@modulo(-123.456, -123.456)), 1)' '0'

executefail '@modulo(100, 0)' 'arithmetic error'
