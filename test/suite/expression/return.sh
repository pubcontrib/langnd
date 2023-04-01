suite 'expression/return'

verify 'import ["write", "freeze"] from core $function=<return null> $write($freeze($function()), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $function=<return false> $write($freeze($function()), 1)' \
    'prints to stdout' 'false'
verify 'import ["write", "freeze"] from core $function=<return 0> $write($freeze($function()), 1)' \
    'prints to stdout' '0'
verify 'import ["write", "freeze"] from core $function=<return ""> $write($freeze($function()), 1)' \
    'prints to stdout' '""'
verify 'import ["write", "freeze"] from core $function=<return []> $write($freeze($function()), 1)' \
    'prints to stdout' '[]'
verify 'import ["write", "freeze"] from core $function=<return {}> $write($freeze($function()), 1)' \
    'prints to stdout' '{}'
verify 'import ["write", "freeze"] from core $function=<return <>> $write($freeze($function()), 1)' \
    'prints to stdout' '<>'
verify '$function=<import "write" from core $write("before", 1) return "value" $write("after", 1)> $function()' \
    'prints to stdout' 'before'
verify 'import ["write", "freeze"] from core $function=<return "first" return "second" return "third"> $write($freeze($function()), 1)' \
    'prints to stdout' '"first"'
verify 'import ["write", "freeze"] from core $function=<while true \ return "value" /> $write($freeze($function()), 1)' \
    'prints to stdout' '"value"'\

verify 'return' \
    'errors with parse message' 'return'

verify 'return "result"' \
    'errors with execute message' '"lost return"'
