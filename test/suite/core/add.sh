suite 'core/add'

verify 'import ["write", "freeze", "add"] from core $write($freeze($add(0, 0)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(1, 2)), 1)' \
    'prints to stdout' '3'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(123, 456)), 1)' \
    'prints to stdout' '579'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(-1, -2)), 1)' \
    'prints to stdout' '-3'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(-123, -456)), 1)' \
    'prints to stdout' '-579'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(0.1, 0.2)), 1)' \
    'prints to stdout' '0.299987'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(0.123, 0.456)), 1)' \
    'prints to stdout' '0.578979'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(-0.1, -0.2)), 1)' \
    'prints to stdout' '-0.299987'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(-0.123, -0.456)), 1)' \
    'prints to stdout' '-0.578979'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(123.456, 123.456)), 1)' \
    'prints to stdout' '246.911987'
verify 'import ["write", "freeze", "add"] from core $write($freeze($add(-123.456, -123.456)), 1)' \
    'prints to stdout' '-246.911987'

verify 'import "add" from core $add(32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "add" from core $add(1, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "add" from core $add(32767, 1)' \
    'errors with execute message' '"arithmetic error"'
