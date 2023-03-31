suite 'core/or'

verify 'import ["write", "freeze", "or"] from core $write($freeze($or(false, false)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "or"] from core $write($freeze($or(false, true)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "or"] from core $write($freeze($or(true, false)), 1)' \
    'prints to stdout' 'true'
verify 'import ["write", "freeze", "or"] from core $write($freeze($or(true, true)), 1)' \
    'prints to stdout' 'true'
