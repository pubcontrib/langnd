suite 'statement/loop'

verify 'while false < @write("HERE", 1) >' \
    'prints to stdout' ''
verify '$i=1 while @precedes($i, 4) < @write(@freeze($i), 1) $i=@add($i, 1) >' \
    'prints to stdout' '123'
verify '$i=1 while @precedes($i, 4) < $j=1 while @precedes($j, 4) < @write(@freeze(@multiply($i, $j)), 1) $j=@add($j, 1) > $i=@add($i, 1) >' \
    'prints to stdout' '123246369'

verify 'while' \
    'errors with parse message' 'while'
verify 'while < >' \
    'errors with parse message' '< >'
verify 'while false <' \
    'errors with parse message' '<'
verify 'while false >' \
    'errors with parse message' 'false >'
verify 'while false 100' \
    'errors with parse message' 'false 100'
verify 'while false < 100' \
    'errors with parse message' '100'

verify 'while null < >' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while 100 < >' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while "text" < >' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while [1, 2, 3] < >' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while {"a": 1, "b": 2, "c": 3} < >' \
    'errors with execute message' '"loop with non-boolean condition"'

verify 'while true < "before" @divide(100, 0) "after" >' \
    'errors with execute message' '"arithmetic error"'
verify 'while @divide(100, 0) < $missing >' \
    'errors with execute message' '"arithmetic error"'
