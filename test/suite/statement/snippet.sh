suite 'statement/snippet'

verify '@write(@freeze(\ /), 1)' \
    'prints to stdout' 'null'
verify '@write(@freeze(\ 1 2 3 /), 1)' \
    'prints to stdout' '3'
verify '@write(@freeze(\ throw "custom error" /), 1)' \
    'errors with execute message' '"custom error"'
verify 'catch \ @write("before", 1) throw "end" @write("after", 1) /' \
    'prints to stdout' 'before'
