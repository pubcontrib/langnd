suite 'function/type'

verify '@write(@type(null), 1)' \
    'prints to stdout' 'NULL'
verify '@write(@type(false), 1)' \
    'prints to stdout' 'BOOLEAN'
verify '@write(@type(0), 1)' \
    'prints to stdout' 'NUMBER'
verify '@write(@type(""), 1)' \
    'prints to stdout' 'STRING'
verify '@write(@type([]), 1)' \
    'prints to stdout' 'LIST'
verify '@write(@type({}), 1)' \
    'prints to stdout' 'MAP'
verify '@write(@type(<>), 1)' \
    'prints to stdout' 'FUNCTION'
