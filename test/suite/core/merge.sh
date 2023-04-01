suite 'core/merge'

verify 'import ["write", "merge"] from core $write($merge("this", " & that"), 1)' \
    'prints to stdout' 'this & that'
verify 'import ["write", "merge"] from core $write($merge("this", ""), 1)' \
    'prints to stdout' 'this'
verify 'import ["write", "merge"] from core $write($merge("", "that"), 1)' \
    'prints to stdout' 'that'
verify 'import ["write", "merge"] from core $write($merge("", ""), 1)' \
    'prints to stdout' ''
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge(["this"], ["that"])), 1)' \
    'prints to stdout' '["this", "that"]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge(["this"], [])), 1)' \
    'prints to stdout' '["this"]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge([], ["that"])), 1)' \
    'prints to stdout' '["that"]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge([], [])), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge(["this", "that"], ["other"])), 1)' \
    'prints to stdout' '["this", "that", "other"]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge(["this"], ["that", "other"])), 1)' \
    'prints to stdout' '["this", "that", "other"]'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({"this": 1}, {"that": 2})), 1)' \
    'prints to stdout' '{"that": 2, "this": 1}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({"this": 1}, {})), 1)' \
    'prints to stdout' '{"this": 1}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({}, {"that": 2})), 1)' \
    'prints to stdout' '{"that": 2}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({}, {})), 1)' \
    'prints to stdout' '{}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({"this": 1, "that": 2}, {"other": 3})), 1)' \
    'prints to stdout' '{"other": 3, "that": 2, "this": 1}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({"this": 1}, {"that": 2, "other": 3})), 1)' \
    'prints to stdout' '{"other": 3, "that": 2, "this": 1}'
verify 'import ["write", "freeze", "merge"] from core $write($freeze($merge({"this": 1}, {"this": 2})), 1)' \
    'prints to stdout' '{"this": 2}'
