suite 'function/merge'

verify '@write(@merge("this", " & that"), 1)' \
    'prints to stdout' 'this & that'
verify '@write(@merge("this", ""), 1)' \
    'prints to stdout' 'this'
verify '@write(@merge("", "that"), 1)' \
    'prints to stdout' 'that'

verify '@merge("this", " & that")' \
    'prints to stdout' ''
