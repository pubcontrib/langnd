suite 'function/not'

verify 'import ["write", "freeze", "not"] from core $write($freeze($not(false)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "not"] from core $write($freeze($not(true)), 1)' \
    'prints to stdout' 'false'
