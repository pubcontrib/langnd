suite 'core/divide'

verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(1, 2)), 1)' \
    'prints to stdout' '0.500000'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(123, 456)), 1)' \
    'prints to stdout' '0.269729'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(-1, -2)), 1)' \
    'prints to stdout' '0.500000'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(-123, -456)), 1)' \
    'prints to stdout' '0.269729'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(0.1, 0.2)), 1)' \
    'prints to stdout' '0.499954'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(0.123, 0.456)), 1)' \
    'prints to stdout' '0.269699'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(-0.1, -0.2)), 1)' \
    'prints to stdout' '0.499954'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(-0.123, -0.456)), 1)' \
    'prints to stdout' '0.269699'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(123.456, 123.456)), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze", "divide"] from core $write($freeze($divide(-123.456, -123.456)), 1)' \
    'prints to stdout' '1'

verify 'import "divide" from core $divide(32767, 0.5)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "divide" from core $divide(32767, -0.5)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "divide" from core $divide(-32767, 0.5)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "divide" from core $divide(-32767, -0.5)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "divide" from core $divide(100, 0)' \
    'errors with execute message' '"arithmetic error"'