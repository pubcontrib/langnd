suite 'statement/branch'

verify 'if false { @write("hit", 1) }' \
    'prints to stdout' ''
verify 'if true { @write("hit", 1) }' \
    'prints to stdout' 'hit'
verify 'if true { if true { @write("hit", 1) } }' \
    'prints to stdout' 'hit'
verify 'if true { if false { @write("hit", 1) } }' \
    'prints to stdout' ''
verify 'if false { if true { @write("hit", 1) } }' \
    'prints to stdout' ''
verify 'if false { if false { @write("hit", 1) } }' \
    'prints to stdout' ''
verify '$a=true if $a { @write("hit", 1) }' \
    'prints to stdout' 'hit'
verify '$a=false if $a { @write("hit", 1) }' \
    'prints to stdout' ''
verify 'if @cast("true", "BOOLEAN") { @write("hit", 1) }' \
    'prints to stdout' 'hit'
verify 'if @cast("false", "BOOLEAN") { @write("hit", 1) }' \
    'prints to stdout' ''
verify '$a=0 if true { $a=@add($a,1) $a=@add($a,1) $a=@add($a,1) } @write(@freeze($a),1)' \
    'prints to stdout' '3'
verify '$msg=if true { "first" "..." "last" } @write(@freeze($msg), 1)' \
    'prints to stdout' '"last"'
verify '$msg=if false { "first" "..." "last" } @write(@freeze($msg), 1)' \
    'prints to stdout' 'null'
verify '@write(@merge("<", if true { " >" }), 1)' \
    'prints to stdout' '< >'
verify 'if true { @write("true", 1) } else { @write("false", 1) }' \
    'prints to stdout' 'true'
verify 'if false { @write("true", 1) } else { @write("false", 1) }' \
    'prints to stdout' 'false'
verify '$msg=if true { "first" } else { "second" } @write(@freeze($msg), 1)' \
    'prints to stdout' '"first"'
verify '$msg=if false { "first" } else { "second" } @write(@freeze($msg), 1)' \
    'prints to stdout' '"second"'

verify 'if' \
    'errors with parse message' 'if'
verify 'if { }' \
    'errors with parse message' '{ }'
verify 'if true' \
    'errors with parse message' 'true'
verify 'if true {' \
    'errors with parse message' '{'
verify 'if true }' \
    'errors with parse message' 'true }'
verify 'if true 100' \
    'errors with parse message' 'true 100'
verify 'if true { 100' \
    'errors with parse message' '100'
verify 'if true { 100 } else' \
    'errors with parse message' 'else'
verify 'if true { 100 } else {' \
    'errors with parse message' '{'
verify 'if true { 100 } else { 200' \
    'errors with parse message' '200'
verify '$a=true if $a=false { }' \
    'errors with parse message' 'false { }'

verify 'if true { "before" @divide(100, 0) "after" }' \
    'errors with execute message' 'arithmetic error'
verify 'if @divide(100, 0) { $missing }' \
    'errors with execute message' 'arithmetic error'
