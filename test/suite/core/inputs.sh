suite 'core/inputs'

verify 'import ["freeze", "write", "inputs"] from core $write($freeze($inputs), 1)' \
    'prints to stdout' '[]'
verify 'import ["freeze", "write", "inputs"] from core $write($freeze($inputs), 1)' \
    'with argv' '1' \
    'with argv' '2' \
    'with argv' '3' \
    'prints to stdout' '["1", "2", "3"]'
verify 'import ["freeze", "write", "inputs"] from core $write($freeze($inputs), 1)' \
    'with argv' 'null' \
    'with argv' 'false' \
    'with argv' '0' \
    'with argv' '""' \
    'with argv' '[]' \
    'with argv' '{}' \
    'with argv' '<>' \
    'prints to stdout' '["null", "false", "0", "\"\"", "[]", "{}", "<>"]'
