suite 'expression/break'

verify 'import "write" from core while true \ $write("a", 1) break "here" $write("b", 1) / $write("c", 1)' \
    'prints to stdout' 'ac'
verify 'import ["write", "freeze"] from core $result=while true \ break "here" / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'import ["write", "freeze"] from core $result=while true \ $pick="here" break $pick / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'import "write" from core while true \ $write("a", 1) catch \ $write("b", 1) break "here" $write("c", 1) / $write("d", 1) / $write("e", 1)' \
    'prints to stdout' 'abe'
verify 'import ["write", "freeze"] from core $result=while true \ catch \ break "here" / / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'import ["write", "freeze"] from core $result=while true \ catch \ $pick="here" break $pick / / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'import ["write", "freeze"] from core $result=while true \ break break "here" / $write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'throw break "here"' \
    'errors with execute message' '"lost break"'
verify 'continue break "here"' \
    'errors with execute message' '"lost break"'

verify 'break' \
    'errors with parse message' 'break'

verify 'break "here"' \
    'errors with execute message' '"lost break"'
