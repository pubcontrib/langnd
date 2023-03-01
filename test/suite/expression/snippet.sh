suite 'expression/snippet'

verify 'import ["write", "freeze"] from core $write($freeze(\ /), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $write($freeze(\ 1 2 3 /), 1)' \
    'prints to stdout' '3'
verify 'import ["write", "freeze"] from core $write($freeze(\ throw "custom error" /), 1)' \
    'errors with execute message' '"custom error"'
verify 'import "write" from core catch \ $write("before", 1) throw "end" $write("after", 1) /' \
    'prints to stdout' 'before'
