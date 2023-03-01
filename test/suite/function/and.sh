suite 'function/and'

verify 'import ["write", "freeze", "and"] from core $write($freeze($and(false, false)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "and"] from core $write($freeze($and(false, true)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "and"] from core $write($freeze($and(true, false)), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze", "and"] from core $write($freeze($and(true, true)), 1)' \
    'prints to stdout' 'true'
