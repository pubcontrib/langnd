suite 'function/subtract'

verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(0, 0)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(1, 2)), 1)' \
    'prints to stdout' '-1'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(123, 456)), 1)' \
    'prints to stdout' '-333'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(-1, -2)), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(-123, -456)), 1)' \
    'prints to stdout' '333'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(0.1, 0.2)), 1)' \
    'prints to stdout' '-0.100006'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(0.123, 0.456)), 1)' \
    'prints to stdout' '-0.333007'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(-0.1, -0.2)), 1)' \
    'prints to stdout' '0.100006'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(-0.123, -0.456)), 1)' \
    'prints to stdout' '0.333007'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(123.456, 123.456)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "subtract"] from core $write($freeze($subtract(-123.456, -123.456)), 1)' \
    'prints to stdout' '0'

verify 'import "subtract" from core $subtract(-32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "subtract" from core $subtract(-1, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "subtract" from core $subtract(-32767, 1)' \
    'errors with execute message' '"arithmetic error"'
