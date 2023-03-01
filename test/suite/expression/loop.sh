suite 'expression/loop'

verify 'import "write" from core while false \ $write("HERE", 1) /' \
    'prints to stdout' ''
verify 'import ["precedes", "write", "freeze", "add"] from core $i=1 while $precedes($i, 4) \ $write($freeze($i), 1) $i=$add($i, 1) /' \
    'prints to stdout' '123'
verify 'import ["precedes", "write", "freeze", "multiply", "add"] from core $i=1 while $precedes($i, 4) \ $j=1 while $precedes($j, 4) \ $write($freeze($multiply($i, $j)), 1) $j=$add($j, 1) / $i=$add($i, 1) /' \
    'prints to stdout' '123246369'
verify 'import ["write", "freeze"] from core $loop=true $result=while $loop \ "first" $loop=false "last" / $write($freeze($result), 1)' \
    'prints to stdout' '"last"'
verify 'import ["write", "freeze"] from core $result=while false \ "first" "..." "last" / $write($freeze($result), 1)' \
    'prints to stdout' 'null'
verify 'import ["write", "freeze"] from core $loop=true $result=while if true \ $loop / \ $loop=false "last" / $write($freeze($result), 1)' \
    'prints to stdout' '"last"'
verify 'import ["write", "freeze"] from core $loop=true $result=while $loop if true \ $loop=false "last" / $write($freeze($result), 1)' \
    'prints to stdout' '"last"'

verify 'while' \
    'errors with parse message' 'while'
verify 'while \ /' \
    'errors with parse message' '/'
verify 'while false \' \
    'errors with parse message' '\'
verify 'while false /' \
    'errors with parse message' '/'
verify 'while false \ 100' \
    'errors with parse message' '100'

verify 'while null \ /' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while 100 \ /' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while "text" \ /' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while [1, 2, 3] \ /' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while {"a": 1, "b": 2, "c": 3} \ /' \
    'errors with execute message' '"loop with non-boolean condition"'
verify 'while <import "multiply" from core $number=argument return $multiply($number, $number)> \ /' \
    'errors with execute message' '"loop with non-boolean condition"'

verify 'import "divide" from core while true \ "before" $divide(100, 0) "after" /' \
    'errors with execute message' '"arithmetic error"'
verify 'import "divide" from core while $divide(100, 0) \ $missing /' \
    'errors with execute message' '"arithmetic error"'
