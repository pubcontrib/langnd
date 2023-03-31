suite 'core/modulo'

verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(1, 2)), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(123, 456)), 1)' \
    'prints to stdout' '123'
verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(-1, -2)), 1)' \
    'prints to stdout' '-1'
verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(-123, -456)), 1)' \
    'prints to stdout' '-123'
verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(123.456, 123.456)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "modulo"] from core $write($freeze($modulo(-123.456, -123.456)), 1)' \
    'prints to stdout' '0'

verify 'import "modulo" from core $modulo(100, 0)' \
    'errors with execute message' '"arithmetic error"'
