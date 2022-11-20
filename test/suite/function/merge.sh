suite 'function/merge'

verify '@write(@merge("this", " & that"), 1)' \
    'prints to stdout' 'this & that'
verify '@write(@merge("this", ""), 1)' \
    'prints to stdout' 'this'
verify '@write(@merge("", "that"), 1)' \
    'prints to stdout' 'that'
verify '@write(@freeze(@merge(["this"], ["that"])), 1)' \
    'prints to stdout' '["this", "that"]'
verify '@write(@freeze(@merge(["this"], [])), 1)' \
    'prints to stdout' '["this"]'
verify '@write(@freeze(@merge([], ["that"])), 1)' \
    'prints to stdout' '["that"]'
verify '@write(@freeze(@merge([], [])), 1)' \
    'prints to stdout' '[]'
verify '@write(@freeze(@merge(["this", "that"], ["other"])), 1)' \
    'prints to stdout' '["this", "that", "other"]'
verify '@write(@freeze(@merge(["this"], ["that", "other"])), 1)' \
    'prints to stdout' '["this", "that", "other"]'
verify '@write(@freeze(@merge({"this": 1}, {"that": 2})), 1)' \
    'prints to stdout' '{"that": 2, "this": 1}'
verify '@write(@freeze(@merge({"this": 1}, {})), 1)' \
    'prints to stdout' '{"this": 1}'
verify '@write(@freeze(@merge({}, {"that": 2})), 1)' \
    'prints to stdout' '{"that": 2}'
verify '@write(@freeze(@merge({}, {})), 1)' \
    'prints to stdout' '{}'
verify '@write(@freeze(@merge({"this": 1, "that": 2}, {"other": 3})), 1)' \
    'prints to stdout' '{"other": 3, "that": 2, "this": 1}'
verify '@write(@freeze(@merge({"this": 1}, {"that": 2, "other": 3})), 1)' \
    'prints to stdout' '{"other": 3, "that": 2, "this": 1}'
verify '@write(@freeze(@merge({"this": 1}, {"this": 2})), 1)' \
    'prints to stdout' '{"this": 2}'
