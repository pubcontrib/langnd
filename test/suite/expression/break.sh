suite 'expression/break'

verify 'while true \ @write("a", 1) break "here" @write("b", 1) / @write("c", 1)' \
    'prints to stdout' 'ac'
verify '$result=while true \ break "here" / @write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify '$result=while true \ $pick="here" break $pick / @write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify 'while true \ @write("a", 1) catch \ @write("b", 1) break "here" @write("c", 1) / @write("d", 1) / @write("e", 1)' \
    'prints to stdout' 'abe'
verify '$result=while true \ catch \ break "here" / / @write($freeze($result), 1)' \
    'prints to stdout' '"here"'
verify '$result=while true \ catch \ $pick="here" break $pick / / @write($freeze($result), 1)' \
    'prints to stdout' '"here"'

verify 'break' \
    'errors with parse message' 'break'

verify 'break "here"' \
    'errors with execute message' '"lost break"'
