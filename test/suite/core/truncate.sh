suite 'core/truncate'

verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(0)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(1)), 1)' \
    'prints to stdout' '1'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(123)), 1)' \
    'prints to stdout' '123'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-1)), 1)' \
    'prints to stdout' '-1'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-123)), 1)' \
    'prints to stdout' '-123'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(32767)), 1)' \
    'prints to stdout' '32767'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-32767)), 1)' \
    'prints to stdout' '-32767'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(0.1)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(0.123)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-0.1)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-0.123)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(123.456)), 1)' \
    'prints to stdout' '123'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-123.456)), 1)' \
    'prints to stdout' '-123'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(0.999999)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-0.999999)), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(32767.999999)), 1)' \
    'prints to stdout' '32767'
verify 'import ["write", "freeze", "truncate"] from core $write($freeze($truncate(-32767.999999)), 1)' \
    'prints to stdout' '-32767'
