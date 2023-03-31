suite 'core/type'

verify 'import ["write", "type"] from core $write($type(null), 1)' \
    'prints to stdout' 'NULL'
verify 'import ["write", "type"] from core $write($type(false), 1)' \
    'prints to stdout' 'BOOLEAN'
verify 'import ["write", "type"] from core $write($type(0), 1)' \
    'prints to stdout' 'NUMBER'
verify 'import ["write", "type"] from core $write($type(""), 1)' \
    'prints to stdout' 'STRING'
verify 'import ["write", "type"] from core $write($type([]), 1)' \
    'prints to stdout' 'LIST'
verify 'import ["write", "type"] from core $write($type({}), 1)' \
    'prints to stdout' 'MAP'
verify 'import ["write", "type"] from core $write($type(<>), 1)' \
    'prints to stdout' 'FUNCTION'
