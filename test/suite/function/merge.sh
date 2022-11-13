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

verify '@merge("this", " & that")' \
    'prints to stdout' ''
