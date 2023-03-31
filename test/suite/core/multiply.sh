suite 'core/multiply'

verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(0, 0)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(1, 2)), 1)' \
    'prints to stdout' '2'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(-1, -2)), 1)' \
    'prints to stdout' '2'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(0.1, 0.2)), 1)' \
    'prints to stdout' '0.019989'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(0.123, 0.456)), 1)' \
    'prints to stdout' '0.056076'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(-0.1, -0.2)), 1)' \
    'prints to stdout' '0.019989'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(-0.123, -0.456)), 1)' \
    'prints to stdout' '0.056076'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(123.456, 123.456)), 1)' \
    'prints to stdout' '15241.382354'
verify 'import ["write", "freeze", "multiply"] from core $write($freeze($multiply(-123.456, -123.456)), 1)' \
    'prints to stdout' '15241.382354'

verify 'import "multiply" from core $multiply(32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "multiply" from core $multiply(32767, -32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "multiply" from core $multiply(-32767, 32767)' \
    'errors with execute message' '"arithmetic error"'
verify 'import "multiply" from core $multiply(-32767, -32767)' \
    'errors with execute message' '"arithmetic error"'
