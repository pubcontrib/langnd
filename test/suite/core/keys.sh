suite 'core/keys'

verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys("")), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys("0")), 1)' \
    'prints to stdout' '[1]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys("0123456789")), 1)' \
    'prints to stdout' '[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys([])), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys([0])), 1)' \
    'prints to stdout' '[1]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys([0, 1, 2, 3, 4, 5, 6, 7, 8, 9])), 1)' \
    'prints to stdout' '[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys({})), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys({"0": 0})), 1)' \
    'prints to stdout' '["0"]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys({"0": 0, "1": 1, "2": 2, "3": 3, "4": 4, "5": 5, "6": 6, "7": 7, "8": 8, "9": 9})), 1)' \
    'prints to stdout' '["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]'
verify 'import ["write", "freeze", "keys"] from core $write($freeze($keys({"null": null, "boolean": false, "number": 0, "string": "", "list": [], "map": {}})), 1)' \
    'prints to stdout' '["boolean", "list", "map", "null", "number", "string"]'
